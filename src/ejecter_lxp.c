/*============================================================================
Copyright (c) 2018-2025 Raspberry Pi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
============================================================================*/

#include <locale.h>
#include <glib/gi18n.h>

#include "plugin.h"

#include "ejecter.h"

/*----------------------------------------------------------------------------*/
/* LXPanel plugin functions                                                   */
/*----------------------------------------------------------------------------*/

/* Constructor */
static GtkWidget *ejecter_constructor (LXPanel *panel, config_setting_t *settings)
{
    /* Allocate and initialize plugin context */
    EjecterPlugin *ej = g_new0 (EjecterPlugin, 1);

    /* Allocate top level widget and set into plugin widget pointer. */
    ej->panel = panel;
    ej->settings = settings;
    ej->plugin = gtk_button_new ();
    lxpanel_plugin_set_data (ej->plugin, ej, ejecter_destructor);

    /* Read config */
    ejecter_set_values (ej);
    lxplug_read_settings (ej->settings, conf_table);

    ejecter_init (ej);

    return ej->plugin;
}

/* Handler for system config changed message from panel */
static void ejecter_configuration_changed (LXPanel *, GtkWidget *plugin)
{
    EjecterPlugin *ej = lxpanel_plugin_get_data (plugin);
    ejecter_update_display (ej);
}

/* Apply changes from config dialog */
static gboolean ejecter_apply_configuration (gpointer user_data)
{
    EjecterPlugin *ej = lxpanel_plugin_get_data (GTK_WIDGET (user_data));
    lxplug_write_settings (ej->settings, conf_table);
    ejecter_update_display (ej);
    return FALSE;
}

/* Display configuration dialog */
static GtkWidget *ejecter_configure (LXPanel *panel, GtkWidget *plugin)
{
    return lxpanel_generic_config_dlg_new (_(PLUGIN_TITLE), panel,
        ejecter_apply_configuration, plugin,
        conf_table);
}

/* Handler for control message */
static gboolean ejecter_control (GtkWidget *plugin, const char *cmd)
{
    EjecterPlugin *ej = lxpanel_plugin_get_data (plugin);
    return ejecter_control_msg (ej, cmd);
}

int module_lxpanel_gtk_version = 1;
char module_name[] = PLUGIN_NAME;

/* Plugin descriptor */
LXPanelPluginInit fm_module_init_lxpanel_gtk = {
    .name = PLUGIN_TITLE,
    .gettext_package = GETTEXT_PACKAGE,
    .description = N_("Ejects mounted drives"),
    .new_instance = ejecter_constructor,
    .reconfigure = ejecter_configuration_changed,
    .config = ejecter_configure,
    .control = ejecter_control
};

/* End of file */
/*----------------------------------------------------------------------------*/
