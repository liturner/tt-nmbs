/// @file nmbs-nautilus.c
/// @brief Primary compilation unit for the NMBS-Nautilus
///
/// @author Luke Ian Turner
/// @date 2026-07-19
/// @copyright Copyright (c) 2026 Luke Ian Turner
/// @copyright
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// @copyright
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// @copyright
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include <nautilus-extension.h>
#include <glib-object.h>
#include <libintl.h>
#include <nmbs/nmbs_c.h>

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

const char* const nmbs_column_classification_key = "nmbs::marking";
const char* const nmbs_property_policy_key = "nmbs::policy";
const char* const nmbs_property_classification_key = "nmbs::classification";
const char* const nmbs_property_originator_key = "nmbs::originator";
const char* const nmbs_file_has_label = "nmbs::file-has-label";
const char* const nmbs_file_supports_label = "nmbs::file-supports-label";

// Little helper for getting our bool style flags out
bool nmbs_get_file_info_bool_attribute(NautilusFileInfo* file, const char* const attribute)
{
    char* attribute_string = nautilus_file_info_get_string_attribute(file, attribute);
    if (attribute_string && strcmp(attribute_string, "TRUE") == 0)
    {
        g_free(attribute_string);
        return true;
    }
    g_free(attribute_string);
    return false;
}

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
        const uint32_t flags = nmbs_binding_flags_read_support(path_str);
        g_log("NMBS", G_LOG_LEVEL_DEBUG, "Flags for %s: %x", path_str, flags);

        // Assume the label is corrupt to start! Gets set to true, first once successfully read.
        nautilus_file_info_add_string_attribute(file, nmbs_file_has_label, "FALSE");
        nautilus_file_info_add_string_attribute(file, nmbs_file_supports_label, "FALSE");

        if (nmbs_binding_flags_supports_labels(flags))
        {
            nautilus_file_info_add_string_attribute(file, nmbs_file_supports_label, "TRUE");
        }

        g_log("NMBS", G_LOG_LEVEL_DEBUG, "Reading Labels for %s", path_str);
        auto const labels = nmbs_confidentiality_labels_new();
        nmbs_confidentiality_labels_read_labels_with_known_binding(labels, path_str, flags);
        g_free(path_str);

        for (unsigned long i = 0; i < nmbs_confidentiality_labels_size(labels); ++i)
        {
            auto const label = nmbs_confidentiality_labels_get(labels, i);
            auto const label_policy = nmbs_confidentiality_label_get_policy(label);
            auto const label_classification = nmbs_confidentiality_label_get_classification(label);
            auto const label_originator = nmbs_confidentiality_label_get_originator_id(label);

            if (label == nullptr || !label_policy || !label_classification)
            {
                continue;
            }

            char* classification = g_strconcat(label_policy, " ", label_classification, NULL);
            if (!classification)
            {
                g_free(classification);
                continue;
            }
            nautilus_file_info_add_string_attribute(file, nmbs_column_classification_key, classification);
            nautilus_file_info_add_string_attribute(file, nmbs_property_policy_key, label_policy);
            nautilus_file_info_add_string_attribute(file, nmbs_property_classification_key, label_classification);
            nautilus_file_info_add_string_attribute(file, nmbs_property_originator_key, label_originator);
            nautilus_file_info_add_string_attribute(file, nmbs_file_has_label, "TRUE");

            g_free(classification);
        }
        nmbs_confidentiality_labels_delete(labels);
    }
    return NAUTILUS_OPERATION_COMPLETE;
}

// Callback used by NautilusColumnProviderInterface::get_columns. Must be fast and should not use
// any blocking IO
static GList* nmbs_properties_get_columns(NautilusColumnProvider*)
{
    NautilusColumn* column = nautilus_column_new(
        "NmbsProperties::classification",
        nmbs_column_classification_key,
        gettext("Classification"),
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

    nmbs_confidentiality_labels_ptr labels = nmbs_confidentiality_labels_new();
    nmbs_confidentiality_label_ptr label = nmbs_confidentiality_labels_emplace_back(labels);
    char* label_policy = tokens[2];
    char* label_classification = tokens[3];
    nmbs_confidentiality_label_set_policy(label, label_policy);
    nmbs_confidentiality_label_set_classification(label, label_classification);

    for (GList* l = files; l != NULL; l = l->next)
    {
        NautilusFileInfo* file = NAUTILUS_FILE_INFO(l->data);
        auto location = nautilus_file_info_get_location(file);
        if (location == NULL)
        {
            continue;
        }
        char* path_str = g_file_get_path(location);
        g_object_unref(location);

        const int return_code = nmbs_confidentiality_labels_write_labels(path_str, labels);
        if (return_code == 0)
        {
            g_log("NMBS", G_LOG_LEVEL_MESSAGE, "Classified %s as %s:%s", path_str, label_policy, label_classification);
        }
        else
        {
            g_log("NMBS", G_LOG_LEVEL_CRITICAL, "Failed to classify %s as %s:%s. Error code: %d", path_str, label_policy, label_classification, return_code);
        }

        g_free(path_str);
    }

    g_strfreev(tokens);
    g_free(item_name);
    nmbs_confidentiality_labels_delete(labels);
}

static void on_clear_classification_item_activated(NautilusMenuItem* menu_item, gpointer user_data)
{
    GList* files = user_data;
    for (GList* l = files; l != NULL; l = l->next)
    {
        NautilusFileInfo* file = NAUTILUS_FILE_INFO(l->data);
        const auto location = nautilus_file_info_get_location(file);
        if (location == NULL)
        {
            continue;
        }
        char* path_str = g_file_get_path(location);
        g_object_unref(location);

        const int return_code = -1;//nmbs_confidentiality_labels_clear_labels(path_str);
        if (return_code == 0)
        {
            g_log("NMBS", G_LOG_LEVEL_MESSAGE, "Cleared Classification from %s", path_str);
        }
        else
        {
            g_log("NMBS", G_LOG_LEVEL_CRITICAL, "Failed to clear Classification on %s. Error code: %d", path_str, return_code);
        }

        g_free(path_str);
    }
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
    if (!nmbs_get_file_info_bool_attribute(file, nmbs_file_supports_label))
    {
        return nullptr;
    }

    GList* items = nullptr;

    NautilusMenuItem* menu_root_item = nautilus_menu_item_new(
        "NMBS:Menu:Root",
        gettext("Classify"),
        "Tag this file with ADatP-4774 classification metadata",
        nullptr
    );

    const auto policies = nmbs_security_policies_new();
    nmbs_security_policies_read_installed(policies);
    NautilusMenu* policy_submenu = nautilus_menu_new();

    for (unsigned long i = 0; i < nmbs_security_policies_size(policies); ++i)
    {
        auto const policy = nmbs_security_policies_get(policies, i);
        auto const policy_name = nmbs_security_policy_get_name(policy);
        auto const policy_classifications = nmbs_security_policy_get_security_classifications(policy);

        char menu_name[128] = "NMBS:Menu:Policy:";
        NautilusMenuItem* policy_submenu_item = nautilus_menu_item_new(
            strncat(menu_name, policy_name, sizeof(menu_name) - strlen(menu_name) - 1),
            policy_name,
            "Tag this file with ADatP-4774 classification metadata",
            nullptr
        );
        nautilus_menu_append_item(policy_submenu, policy_submenu_item);
        NautilusMenu* classification_submenu = nautilus_menu_new();
        nautilus_menu_item_set_submenu(policy_submenu_item, classification_submenu);

        for (unsigned long j = 0; j < nmbs_security_classifications_size(policy_classifications); ++j)
        {
            auto const policy_classification = nmbs_security_classifications_get(policy_classifications, j);
            auto const policy_classification_name = nmbs_security_classification_get_name(policy_classification);

            char classification_name[128] = "NMBS:Classification:";
            strcat(classification_name, policy_name);
            strcat(classification_name, ":");
            strcat(classification_name, policy_classification_name);
            NautilusMenuItem* classification_submenu_item = nautilus_menu_item_new(
                classification_name,
                policy_classification_name,
                "Tag this file with ADatP-4774 classification metadata",
                nullptr
            );
            nautilus_menu_append_item(classification_submenu, classification_submenu_item);
            g_signal_connect_data(
                classification_submenu_item,
                "activate",
                G_CALLBACK(on_classify_item_activated),
                g_list_copy_deep(files, ref_count_increment, NULL),
                ref_count_decrement,
                G_CONNECT_DEFAULT
            );
        }
    }
    nautilus_menu_item_set_submenu(menu_root_item, policy_submenu);
    nmbs_security_policies_delete(policies);
    items = g_list_append(items, menu_root_item);

    //
    // Clear Classification Button
    //
    if (nmbs_get_file_info_bool_attribute(file, nmbs_file_has_label))
    {
        NautilusMenuItem* clear_classification_button = nautilus_menu_item_new(
            "NMBS:Menu:Policy",
            gettext("Clear Classification"),
            "Tag this file with ADatP-4774 classification metadata",
            nullptr
        );
        g_signal_connect_data(
            clear_classification_button,
            "activate",
            G_CALLBACK(on_clear_classification_item_activated),
            g_list_copy_deep(files, ref_count_increment, NULL),
            ref_count_decrement,
            G_CONNECT_DEFAULT
        );
        items = g_list_append(items, clear_classification_button);
    }

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
    if (!nmbs_get_file_info_bool_attribute(file, nmbs_file_has_label))
    {
        return items;
    }

    char* policy_identifier = nautilus_file_info_get_string_attribute(file, nmbs_property_policy_key);
    char* classification = nautilus_file_info_get_string_attribute(file, nmbs_property_classification_key);
    char* originator_id = nautilus_file_info_get_string_attribute(file, nmbs_property_originator_key);

    if (!(policy_identifier && classification))
    {
        return nullptr;
    }

    GListStore* model_properties = g_list_store_new(NAUTILUS_TYPE_PROPERTIES_ITEM);
    NautilusPropertiesItem* policy_property = nautilus_properties_item_new(gettext("Policy Identifier"), policy_identifier);
    NautilusPropertiesItem* classification_property = nautilus_properties_item_new(gettext("Classification"), classification);
    NautilusPropertiesItem* creation_time_property = nautilus_properties_item_new(gettext("Classified On"), "");
    NautilusPropertiesItem* binding_property = nautilus_properties_item_new(gettext("Binding Profile"), ""); // TODO: The full URN here! Including version

    g_list_store_append(model_properties, policy_property);
    g_list_store_append(model_properties, classification_property);
    g_list_store_append(model_properties, creation_time_property);
    g_list_store_append(model_properties, binding_property);

    g_object_unref(policy_property);
    g_object_unref(classification_property);
    g_object_unref(creation_time_property);
    g_object_unref(binding_property);

    if (originator_id)
    {
        NautilusPropertiesItem* originator_property = nautilus_properties_item_new(gettext("Classified By"), "");
        g_list_store_append(model_properties, originator_property);
        g_object_unref(originator_property);
    }

    NautilusPropertiesModel* classification_model = nautilus_properties_model_new(gettext("Classification"), G_LIST_MODEL(model_properties));
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
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "LANG: %s", getenv("LANG"));
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "NMBS_LOCPATH: %s", getenv("NMBS_LOCPATH"));

    nmbs_properties_register_type(module);

    g_log("NMBS", G_LOG_LEVEL_DEBUG, "bindtextdomain: %s", bindtextdomain("nmbs", getenv("NMBS_LOCPATH")));
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "textdomain: %s", textdomain("nmbs"));
}

void nautilus_module_shutdown(void)
{
    g_log("NMBS", G_LOG_LEVEL_DEBUG, "nautilus_module_shutdown");
    nmbs_cleanup();
}

void nautilus_module_list_types(const GType** types, int* num_types)
{
    static GType extension_types[1];
    extension_types[0] = nmbs_properties_get_type();
    *types = extension_types;
    *num_types = 1;
}
