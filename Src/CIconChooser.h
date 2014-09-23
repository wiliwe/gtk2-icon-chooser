/*! \file    CIconChooser.h
    \brief   Declaration of class CIconChooser.

    \author  William.L
    \date    2008-09-26
    \version 1.0

    \b Change_History: 
    \n 1) 2008-09-26 William.L initialize. 
*/

#ifndef __CICONCHOOSER
#define __CICONCHOOSER

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

/* Default icon path. This is used for file chooser, also */
#define DEFAULT_ICON_PATH  "/usr/share/pixmaps/"
#define DEFAULT_ICON_PATH_2  "/usr/share/app-install/icons/"

#define DEFAULT_ICON    DEFAULT_ICON_PATH"gnome-gimp.png"
#define DEFAULT_ICON_2  DEFAULT_ICON_PATH_2"gimp.xpm"

/* The alternative icons searching path. */
#define ICON_SEARCH_PATH_PIXMAPS             "pixmaps"
#define ICON_SEARCH_PATH_HICOLOR             "icons/hicolor"
#define ICON_SEARCH_PATH_HICOLOR_SCALABLE    "icons/hicolor/scalable/apps"
#define ICON_SEARCH_PATH_GNOME               "icons/gnome"
#define ICON_SEARCH_PATH_GNOME_SCALABLE      "icons/gnome/scalable"
#define ICON_SEARCH_PATH_GNOME_SCALABLE_APPS "icons/gnome/scalable/apps"

/* Extension names of images. */
#define   EXT_NAME_PNG  ".png"
#define   EXT_NAME_XPM  ".xpm"
#define   EXT_NAME_SVG  ".svg"

/*! \enum ICONCHOOSER_WIDGET_IDX
    \brief The widget index
*/
enum ICONCHOOSER_WIDGET_IDX {
  ICONCHOOSER_GtkWindow_Main = 0,
  ICONCHOOSER_GtkIconView,
  ICONCHOOSER_GtkTreeSelection,
  ICONCHOOSER_GtkListStore,
  ICONCHOOSER_GtkEntry_IconPathName,
  ICONCHOOSER_GtkButton_BrowseIcon,
  ICONCHOOSER_GtkEntry_IconTotal,
  ICONCHOOSER_GtkEntry_VisibleIconTotal,
  N_ICONCHOOSER_WIDGET_IDX
};

/*! \class CIconChooser
    \brief The Icon Chooser class.
*/
class CIconChooser
{
  private:
    GtkWidget *m_pwParent;  /*!< Store the pointer to the parent(top-level) window. */
    GtkWidget *m_IconView;
    int m_nMaxWidth;  /*!< The width of the Icon Chooser window. */
    int m_nMaxHeight; /*!< The height of the Icon Chooser window. */
    gchar *m_CurrentIcon;        /*!< The currently wanted icon's name including path. When the icon-chooser is invoked, this will be set to the current used icon's path.*/
    gchar *m_IconBrowseLocation; /*!< The current location for browsing icons. */
    gchar *m_DefaultIconPath;    /*!< The default icon searching path. */
    gchar *m_DefaultIcon;        /*!< The default icon full name. */
    gint m_nIconWidth;
    gint m_nIconHeight;
    gint m_nButtonWidth;
    gint m_nButtonHeight;
    gboolean m_bIsChosen;  /*!< To indicate if an icon is chosen. */

    /* GtkIconView Relevant variables */
    GtkTreeSelection *m_TreeSelection;       /*!< The selection instance gotten from the created icon view. */
    GtkListStore *m_ListStore;               /*!< The GtkListStore type memer variable. */
    GtkTreeIter m_TreeIter, m_ChildNodeIter; /*!< The tree iterate objects represent top-level and child nodes respectively. */
    GtkWidget *m_pWidgets[N_ICONCHOOSER_WIDGET_IDX];  /*!< This is used to store widget instances for accessing in the event handle callback function. */
    gint m_icon_total;         /*!< Total number of icons in a chosen directory. */
    gint m_icon_visible_total; /*!< Total number of icons could be shown in icon view in a chosen directory. */

  public:
    CIconChooser(gchar *currentIconFullName, GtkWidget *pwGtkParent);
    ~CIconChooser();

    void m_CreateInitValue(gchar *currentIconFullName);
    gboolean m_InitLayoutUI(GtkWidget *pwGtkParent);
    gboolean m_DoModal(void);  /*!< This function is only for dialog window  */
    void m_DeinitValue(void);
    void m_GetWindowSize(int &nWidth, int &nHeight);

    GtkWidget* m_GetWidget(int idx) { return m_pWidgets[idx]; } /*!< To get the widget object */

    GtkTreeModel* m_CreateAndFillModel (void);
    GtkWidget* m_CreateIconView (void);                 

    /* The getting/setting function for the default icons searchin path. */
    void m_SetDefaultIcon(gchar *defaultIcon);
    gchar* m_GetDefaultIcon() { return m_DefaultIcon; } 

    /* The getting/setting function for the default icons searching path. */
    void m_SetDefaultIconPath(gchar *defaultIconPath);
    gchar* m_GetDefaultIconPath() { return m_DefaultIconPath; } 

    /* The getting/setting function for the currently chosen icon full name. */
    void m_SetCurrentIcon(gchar *icon) ; 
    gchar* m_GetCurrentIcon() { return m_CurrentIcon; } 

    /* The getting/setting function for the current location for browsing icons. */
    void m_SetIconBrowseLocation(gchar *iconlocation);
    gchar* m_GetIconBrowseLocation() { return m_IconBrowseLocation; } 

    /* To load/reload icon list for the new chosen icon path. */   
    gboolean m_LoadIconList(void);
    gboolean m_ReloadIconList(gchar  *iconpath);

    /* To get/set the flag indicating if there has any select action had been done. */
    void  m_SetIsChosen(gboolean chosen) { m_bIsChosen = chosen; }
    gboolean m_GetIsChosen(void) { return m_bIsChosen; }

    /* Icon loading functions. */
    GdkPixbuf* m_LoadIcon( const gchar* name, gint size, gboolean use_fallback );   /*!< To load a icon's image contents. */
    GdkPixbuf* m_LoadIconFile( const char* file_name, int size );
    GdkPixbuf* m_LoadThemeIcon( GtkIconTheme* theme, const char* icon_name, int size );
    gchar* m_GetIconFullName(const char* file_name, int size);

    gboolean  m_IsPhotoFile (gchar *pFile);   /*!< To filt valid format of the icon.*/

    void InitIconChooserDlg(int nPosX, int nPosY);

    void m_SetIconWidth(gint iconWidth) { if(iconWidth>0 && iconWidth<m_nButtonWidth)  m_nIconWidth = iconWidth; }
    gint m_GetIconWidth(void) { return m_nIconWidth; }

    void m_SetIconHeight(gint iconHeight) { if(iconHeight>0 && iconHeight<m_nButtonHeight)  m_nIconHeight = iconHeight; }
    gint m_GetIconHeight(void) { return m_nIconHeight; }

    void m_SetButtonWidth(gint bolderWidth) { if(bolderWidth>0)  m_nButtonWidth = bolderWidth; }
    gint m_GetButtonWidth(void) { return m_nButtonWidth; }
        
    void m_SetButtonHeight(gint bolderHeight) { if(bolderHeight>0)  m_nButtonHeight = bolderHeight; }
    gint m_GetButtonHeight(void) { return m_nButtonHeight; }

    void m_UpdateIconLocation(void);	
    void m_RemoveOldTreeModel(gboolean isDeinit);

    gint m_GetIconTotalNum(void) { return m_icon_total; }
    gint m_GetIconVisiableTotalNum(void) { return m_icon_visible_total; }
};
#endif   /* CICONCHOOSER.H	*/

