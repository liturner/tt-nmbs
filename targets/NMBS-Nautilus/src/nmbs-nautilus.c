#include <nautilus-extension.h>
#include <glib-object.h>

#include "nmbs/c/nmbs.h"

/*
 *
 * Nautilus and glib boilerplate
 *
 */

G_DECLARE_FINAL_TYPE(NmbsProperties, nmbs_properties, NMBS, PROPERTIES, GObject)

struct _NmbsProperties
{
    GObject parent_instance;
};


static void nmbs_properties_column_provider_iface_init(NautilusColumnProviderInterface* iface);
static void nmbs_properties_info_provider_iface_init(NautilusInfoProviderInterface* iface);
static void nmbs_properties_menu_provider_iface_init(NautilusMenuProviderInterface* iface);
static void nmbs_properties_model_provider_iface_init(NautilusPropertiesModelProviderInterface* iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(
    NmbsProperties, nmbs_properties, G_TYPE_OBJECT, 0,
    G_IMPLEMENT_INTERFACE_DYNAMIC(NAUTILUS_TYPE_COLUMN_PROVIDER, nmbs_properties_column_provider_iface_init)
    G_IMPLEMENT_INTERFACE_DYNAMIC(NAUTILUS_TYPE_INFO_PROVIDER, nmbs_properties_info_provider_iface_init)
    G_IMPLEMENT_INTERFACE_DYNAMIC(NAUTILUS_TYPE_MENU_PROVIDER, nmbs_properties_menu_provider_iface_init)
    G_IMPLEMENT_INTERFACE_DYNAMIC(NAUTILUS_TYPE_PROPERTIES_MODEL_PROVIDER, nmbs_properties_model_provider_iface_init)
)

/*
 *
 * Our logic code
 *
 */

const char* const nmbs_column_classification_key = "nmbs::policy-classification";
const char* const nmbs_property_policy_key = "nmbs::policy";
const char* const nmbs_property_classification_key = "nmbs::classification";
const char* const nmbs_file_supports_xmp = "nmbs::file-supports-xmp";
const char* const nmbs_file_has_xmp = "nmbs::file-has-xmp";

// NautilusInfoProvider Implementation
// This would be a long-running function. Nautilus runs it async so as not to slow the UI
static NautilusOperationResult nmbs_properties_update_file_info(
    NautilusInfoProvider*,
    NautilusFileInfo* file,
    GClosure*,
    NautilusOperationHandle**)
{
    auto location = nautilus_file_info_get_location(file);
    if (location == NULL)
    {
        return NAUTILUS_OPERATION_COMPLETE;
    }

    char* path_str = g_file_get_path(location);
    g_object_unref(location);

    if (path_str)
    {
        const uint32_t flags = nmbs_binding_support(path_str);
        g_log("NMBS", G_LOG_LEVEL_DEBUG, "Flags for %s: %x", path_str, flags);

        nautilus_file_info_add_string_attribute(file, nmbs_file_supports_xmp, flags & nmbs_binding_supports_xmp ? "TRUE" : "FALSE" );
        nautilus_file_info_add_string_attribute(file, nmbs_file_has_xmp, flags & nmbs_binding_has_xmp ? "TRUE" : "FALSE" );

        if (flags & nmbs_binding_has_xmp)
        {
            nmbs_confidentiality_labels labels = nmbs_read_labels(path_str);
            // Should never happen. Indicates the Xmp key for a binding existed, but no label was in the binding. Definate error!
            if (labels.size <= 0)
            {
                g_free(path_str);
                return NAUTILUS_OPERATION_COMPLETE;
            }

            for (int i = 0; i < labels.size; i++)
            {
                if (!(labels.label[i].policy_identifier && labels.label[i].classification))
                {
                    g_free(path_str);
                    return NAUTILUS_OPERATION_COMPLETE;
                }

                char* classification = g_strconcat(labels.label[i].policy_identifier, ":",
                                                   labels.label[i].classification, NULL);
                if (!classification)
                {
                    g_free(classification);
                    continue;
                }
                nautilus_file_info_add_string_attribute(file, nmbs_column_classification_key, classification);
                nautilus_file_info_add_string_attribute(file, nmbs_property_policy_key, labels.label[i].policy_identifier);
                nautilus_file_info_add_string_attribute(file, nmbs_property_classification_key, labels.label[i].classification);

                g_free(classification);
            }

            nmbs_free_confidentiality_labels(&labels);
        }
    }

    g_free(path_str);
    return NAUTILUS_OPERATION_COMPLETE;
}

// Callback used by NautilusColumnProviderInterface::get_columns. Must be fast and should not use
// any blocking IO
static GList* nmbs_properties_get_columns(NautilusColumnProvider*)
{
    NautilusColumn* column = nautilus_column_new(
        "NmbsProperties::classification",
        nmbs_column_classification_key,
        "Classification",
        "Displays ADatP-4774 classification metadata"
    );

    GList* columns = nullptr;
    columns = g_list_append(columns, column);
    return columns;
}

static void on_classify_item_activated(NautilusMenuItem* menu_item, gpointer user_data)
{
    GList* files = user_data;

    char* item_name = nullptr;
    g_object_get(G_OBJECT(menu_item), "name", &item_name, NULL);
    char **tokens = g_strsplit(item_name, ":", 4);

    nmbs_confidentiality_labels labels;
    nmbs_confidentiality_label label;
    label.policy_identifier = tokens[2];
    label.classification = tokens[3];
    labels.size = 1;
    labels.label = &label;

    for (GList* l = files; l != NULL; l = l->next)
    {
        NautilusFileInfo* file = NAUTILUS_FILE_INFO(l->data);
        char* uri = nautilus_file_info_get_uri(file);

        const int return_code = nmbs_write_labels(uri, &labels);
        if (return_code == 0)
        {
            g_log("NMBS", G_LOG_LEVEL_MESSAGE, "Classified %s as %s:%s", uri, label.policy_identifier, label.classification);
        }
        else
        {
            g_log("NMBS", G_LOG_LEVEL_CRITICAL, "Failed to classify %s as %s:%s. Error code: %d", uri, label.policy_identifier, label.classification, return_code);
        }

        g_free(uri);
    }

    g_strfreev(tokens);
    g_free(item_name);
}

/// This little guy just exists as the normal use throws warnings. The glib API was a little dirty
/// so we wrap it cleanly.
/// @param src
/// @return
static gpointer ref_count_increment(const gconstpointer src, gpointer)
{
    return g_object_ref((gpointer)src);
}

static void ref_count_decrement(gpointer data, GClosure*)
{
    g_list_free_full(data, g_object_unref);
}

static GList* nmbs_properties_get_file_items(
    NautilusMenuProvider*,
    GList* files)
{
    // Don't show the menu item if no files are selected
    if (files == NULL)
    {
        return nullptr;
    }

    // No multiple selection support!
    if (g_list_length(files) != 1)
    {
        return nullptr;
    }

    NautilusFileInfo* file = NAUTILUS_FILE_INFO(files->data);
    char* file_supports_xmp = nautilus_file_info_get_string_attribute(file, nmbs_file_supports_xmp);
    if (!file_supports_xmp)
    {
        return nullptr;
    }

    if (strcmp(file_supports_xmp, "TRUE") != 0)
    {
        g_free(file_supports_xmp);
        return nullptr;
    }
    g_free(file_supports_xmp);

    NautilusMenuItem* menu_root_item = nautilus_menu_item_new(
        "NMBS:Menu:Root",
        "Classify",
        "Tag this file with ADatP-4774 classification metadata",
        nullptr
    );

    // TODO: This should be libnmbs driven, with a "get spif policy identifiers" function
    NautilusMenuItem* menu_policy_item = nautilus_menu_item_new(
        "NMBS:Menu:Policy:PUBLIC",
        "PUBLIC",
        "Tag this file with ADatP-4774 classification metadata",
        nullptr
    );

    NautilusMenuItem* menu_classification_item = nautilus_menu_item_new(
        "NMBS:Classification:PUBLIC:UNMARKED",
        "UNMARKED",
        "Tag this file with ADatP-4774 classification metadata",
        nullptr
    );

    g_signal_connect_data(
        menu_classification_item,
        "activate",
        G_CALLBACK(on_classify_item_activated),
        g_list_copy_deep(files, ref_count_increment, NULL),
        ref_count_decrement,
        G_CONNECT_DEFAULT
    );

    NautilusMenu* policy_submenu = nautilus_menu_new();
    nautilus_menu_append_item(policy_submenu, menu_policy_item);

    NautilusMenu* classification_submenu = nautilus_menu_new();
    nautilus_menu_append_item(classification_submenu, menu_classification_item);

    nautilus_menu_item_set_submenu(menu_root_item, policy_submenu);
    nautilus_menu_item_set_submenu(menu_policy_item, classification_submenu);

    GList* items = nullptr;
    items = g_list_append(items, menu_root_item);
    return items;
}

///
/// @param provider
/// @param files
/// @return A list of NautilusPropertiesModel* (The caller of the method takes ownership of the returned data)
static GList* nmbs_properties_get_models(NautilusPropertiesModelProvider*, GList* files)
{
    if (g_list_length(files) != 1)
    {
        // No multiple selection support!
        return nullptr;
    }
    NautilusFileInfo* file = NAUTILUS_FILE_INFO(files->data);
    GList* items = nullptr;

    char* file_has_xmp = nautilus_file_info_get_string_attribute(file, nmbs_file_has_xmp);
    if (!file_has_xmp)
    {
        return items;
    }

    if (strcmp(file_has_xmp, "TRUE") != 0)
    {
        g_free(file_has_xmp);
        return items;
    }
    g_free(file_has_xmp);

    char* policy_identifier = nautilus_file_info_get_string_attribute(file, nmbs_property_policy_key);
    char* classification = nautilus_file_info_get_string_attribute(file, nmbs_property_classification_key);

    if (!(policy_identifier && classification))
    {
        return nullptr;
    }

    GListStore* model_properties = g_list_store_new(NAUTILUS_TYPE_PROPERTIES_ITEM);
    NautilusPropertiesItem* policy_property = nautilus_properties_item_new("Policy Identifier", policy_identifier);
    NautilusPropertiesItem* classification_property = nautilus_properties_item_new("Classification", classification);

    g_list_store_append(model_properties, policy_property);
    g_list_store_append(model_properties, classification_property);

    g_object_unref(policy_property);
    g_object_unref(classification_property);

    NautilusPropertiesModel* classification_model = nautilus_properties_model_new("Classification", G_LIST_MODEL(model_properties));
    g_object_unref(model_properties);

    items = g_list_append(items, classification_model);

    g_free(policy_identifier);
    g_free(classification);

    return items;
}

/*
 *
 * Registered initialiser functions (part of the Macros above).
 *
 */

static void nmbs_properties_init(NmbsProperties*)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_init");
}

static void nmbs_properties_class_init(NmbsPropertiesClass*)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_class_init");
}

static void nmbs_properties_class_finalize(NmbsPropertiesClass*)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_class_finalize");
}

static void nmbs_properties_info_provider_iface_init(NautilusInfoProviderInterface* iface)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_info_provider_iface_init");
    iface->update_file_info = nmbs_properties_update_file_info;
}

static void nmbs_properties_column_provider_iface_init(NautilusColumnProviderInterface* iface)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_column_provider_iface_init");
    iface->get_columns = nmbs_properties_get_columns;
}

static void nmbs_properties_menu_provider_iface_init(NautilusMenuProviderInterface* iface)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_menu_provider_iface_init");
    iface->get_file_items = nmbs_properties_get_file_items;
}

static void nmbs_properties_model_provider_iface_init(NautilusPropertiesModelProviderInterface* iface) {
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nmbs_properties_model_provider_iface_init");
    iface->get_models = nmbs_properties_get_models;
}

/*
 *
 * Nautilus Extension Entry Points.
 *
 */

void nautilus_module_initialize(GTypeModule* module)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nautilus_module_initialize");
    nmbs_properties_register_type(module);
}

void nautilus_module_shutdown(void)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nautilus_module_shutdown");
    // Free memory if necessary
}

void nautilus_module_list_types(const GType** types, int* num_types)
{
    static GType extension_types[1];
    extension_types[0] = nmbs_properties_get_type();
    *types = extension_types;
    *num_types = 1;
}
