/*! \file    CIconChooser.cpp
    \brief   Provide the user an UI to select icon and get the full path of the chosen icon file.

    \author  William.L
    \date    2008-09-26
    \version 1.0

    \b Change_History: 
    \n 1. 2008-09-26 William.L initial version. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n.h>  /* For multi-language. */

#include "CIconChooser.h"

/* The title of the frame widget. The tile string is enclosed with GNU gettext hint for multi-language. */
#define WINDOW_TITLE  _("Icon Chooser")

/* The default icon to show. */
#define  DEFAULT_APP_ICON        "application-x-executable"
#define  DEFAULT_APP_MIME_ICON  "gnome-mime-application-x-executable"

/* The size of the icon shown in the icon view. The unit is "pixel" */
#define IMG_SIZE 48

/* The maximum number of characters of one menu item's name and comment. */
#define  MAX_ICON_PATH 2048

/* The dimension of main window. */
#define MAIN_WIN_WIDTH  540
#define MAIN_WIN_HEIGHT 400

/* The dimension of scroll window. */
#define SCROLL_WIDTH  520
#define SCROLL_HEIGHT 300

/* The dimension of text entry widget. */
#define TEXT_ENTRY_WIDTH  430
#define TEXT_ENTRY_HEIGHT 25

/* The dimension of the file chooser/dialog. */
#define FILE_DIALOG_WIDTH  450 
#define FILE_DIALOG_HEIGHT 400

/* The dimension of the icon on the icon-button. */
#define ICON_SHOW_WIDTH  64
#define ICON_SHOW_HEIGHT 64

/* The dimension of the icon-button. */
#define ICON_BUTTON_WIDTH  80
#define ICON_BUTTON_HEIGHT 80

/* The resolution of the screen of your computer  */
#define MAX_WIDTH  1024
#define MAX_HEIGHT 576


/*! \enum ICON_ITEM_IDX 
    \brief The application items icon view column.
*/
enum ICON_ITEM_IDX 
{
  COLUMN_ICON = 0,
  COLUMN_ICONNAME,
  COLUMN_ICONPATH,
  NUM_COLS
};

//------------------------ Callback Functions
#ifdef USE_FILECHOOSER
/*! \fn static void on_browse_icon_path(GtkButton *button, CIconChooser *thisObject)
    \brief The callback function for selecting icons locating path.

    \param[in] button. The GtkButton object this callback function connects to.
    \param[in] thisObject. The instance of class CIconChooser.
    \return NON
*/
static void on_browse_icon_path(GtkButton *button, CIconChooser *thisObject)
{
  GtkWidget  *dialog =NULL;

  if(!button || !thisObject)
    return;

  /* Create a file dialog and set it that the user could only browse directories. */
  dialog = gtk_file_chooser_dialog_new( _("Select Icon Folder ..."),
                                        GTK_WINDOW (thisObject->m_GetWidget(ICONCHOOSER_GtkWindow_Main)),
                                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);

  /* To set the default folder of icons for choosing wanted icons. */
  {
     /* To get the current entered text. */
	 gchar *text = (gchar*)gtk_entry_get_text((GtkEntry*)thisObject->m_GetWidget(ICONCHOOSER_GtkEntry_IconPathName));
	 gchar *dirName = NULL;			
			
     if(text)
       dirName = g_path_get_dirname(text);
				
     if(dirName)
     {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), dirName);
        g_free(dirName);
     }
     else
     {   
        /* To use the default icons searching path. */
        if ( thisObject->m_GetDefaultIconPath() )
          gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), thisObject->m_GetDefaultIconPath() );
        else
          gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), DEFAULT_ICON_PATH);
     }
  }

  /* To resize the default size of the file choosing dialog. */
  gtk_window_resize((GtkWindow*)dialog, FILE_DIALOG_WIDTH, FILE_DIALOG_HEIGHT);

  /* Sets whether hidden files and folders are displayed in the file selector. */
  gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER (dialog), FALSE);

  /* To run the file dialog. And to store the selected files/directory. */
  if( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT )
  {
     /* To retrieve the selected command executable. */
     GSList *selectCmd = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
     gchar *chosenDir = g_strdup_printf("%s%s", (gchar*)(selectCmd->data), "/");

     /* To refrsh/update the icon list store(model). */
     gtk_entry_set_text( (GtkEntry*)thisObject->m_GetWidget(ICONCHOOSER_GtkEntry_IconPathName), chosenDir);

     if(chosenDir)
       g_free(chosenDir);

     chosenDir = NULL;
  }

  /* Destroy the file dialog widget. */
  gtk_widget_destroy (dialog);
}
#endif

/*!	\fn static gboolean on_file_apply(GtkButton *button, CIconChooser *thisObject)
    \brief The callback function to retrieve user wanting node-data from the selected node.

    \param[in] button. The GtkButton object this callback function connects to.
    \param[in] thisObject. The instance of class CIconChooser.
    \return TRUE or FALSE
*/
static gboolean on_file_apply(GtkButton *button, CIconChooser *thisObject)
{		
  gchar *text = NULL;

  if(!button || !thisObject)
    return false;

  /* To get the current entered text. */
  text = (gchar*)gtk_entry_get_text((GtkEntry*)thisObject->m_GetWidget(ICONCHOOSER_GtkEntry_IconPathName));

  /* To store the current selected icon's full name. */
  if( g_file_test(text, (GFileTest)(G_FILE_TEST_EXISTS|G_FILE_TEST_IS_REGULAR)) )
  {
    thisObject->m_SetCurrentIcon(text);
  }

  /* To set the flag to indicate the user had chosen a icon. */
  thisObject->m_SetIsChosen(true);

  /* To deinitialize all member variables and release allocated memory. */
  thisObject->m_DeinitValue();

  /* To destroy dialog window. */
  gtk_widget_destroy( thisObject->m_GetWidget(ICONCHOOSER_GtkWindow_Main) );

  return true;
}

/*!	\fn on_close(GtkButton *button, CIconChooser *thisObject)
    \brief The callback function just to close the applications chooser and do nothing.

    \param[in] button. The GtkButton object this callback function connects to.
    \param[in] thisObject. The instance of class CIconChooser.
    \return TRUE or FALSE
*/
static gboolean on_close(GtkButton *button, CIconChooser *thisObject)
{		
  if(!button || !thisObject)
    return false;
		
  /* To de-initialize all member variables and release allocated memory. */
  thisObject->m_DeinitValue();

  /* To destroy dialog window. */
  gtk_widget_destroy( thisObject->m_GetWidget(ICONCHOOSER_GtkWindow_Main) );	

  return true;
}

/*!	\fn static void cb_selection_changed (GtkIconView *iconView, CIconChooser *thisObject)
    \brief The callback function for processing icon view item selected events.

    \param[in] iconView. The GtkIconView object this callback function connects to.
    \param[in] thisObject. The instance of class CIconChooser.
    \return NONE
*/
static void cb_selection_changed(GtkIconView *iconView, CIconChooser *thisObject)
{
  GList *selected = NULL;
  GtkTreeIter iter;
  GtkTreeModel *model = NULL;
  GtkTreePath *path = NULL;
  GtkIconView *iconview = NULL;
  gchar *iconFullName = NULL;

#ifdef DEBUG_MENU_ICONCHOOSER
  g_print ("Selection changed!\n");
#endif

  if(!iconView || !thisObject)
    return;

  /* To get the icon view object. */
  iconview = (GtkIconView*)thisObject->m_GetWidget(ICONCHOOSER_GtkIconView);

  /* To get the path to the currently selected icon object in the icon view. */
  selected = gtk_icon_view_get_selected_items(iconview);

  /* To get the model of the icon view. */
  model = gtk_icon_view_get_model(iconview);

  /* Convert the path data from the "data" field in the GList node. */
  if(selected && selected->data)
     path = (GtkTreePath*)selected->data;

  /* To get the iterate from the path to the selected icon object. */
  if( (path != NULL) && (gtk_tree_model_get_iter(model, &iter, path) == true)  )
  {
     /* To retrieve the full name of the selected icon object. */
     gtk_tree_model_get(model, &iter, COLUMN_ICONPATH, &iconFullName,  -1);

     /* To set the selected icon's full name to the text entry. */
     gtk_entry_set_text((GtkEntry*)thisObject->m_GetWidget(ICONCHOOSER_GtkEntry_IconPathName), iconFullName);
  }

  if(selected)
    g_list_free(selected);
}

/*!	\fn static void cb_text_changed(GtkEditable *iconView, CIconChooser *thisObject)
    \brief The callback function for processing entry text changed events.

    \param[in] textentry. The GtkEntry object.
    \param[in] thisObject. The Icon Chooser window instance.
    \return NONE
*/
static void cb_text_changed(GtkEditable *textentry, CIconChooser *thisObject)
{	
  gchar *text =NULL;

  if(!textentry || !thisObject)
    return;

  /* To get the current entered text. */
  text = (gchar*)gtk_entry_get_text((GtkEntry*)textentry);

  /* To update the icon list contents if the entered text is a directory. */
  if( (g_file_test(text, (GFileTest) (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) == true) &&
      (g_file_test(text, (GFileTest) (G_FILE_TEST_IS_DIR)) == false)  )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER  
     printf("\n %s(%d) It is an exisent regular file but not a directory. \n", __FUNCTION__, __LINE__)
     #endif
     ;
  }
  else if( g_file_test(text, (GFileTest)(G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) == true )
  {
     gchar *pathName = NULL;

     if(g_str_has_suffix(text, "/") == false)
     {
        pathName = g_strdup_printf("%s%s", text, "/");
        thisObject->m_ReloadIconList(pathName);

        if(pathName)
          g_free(pathName);
     }
     else
     {
        /* To set the gotten entered text. */
        thisObject->m_ReloadIconList(text);
     }
   }
}

//--------------- Class Member Function Implementation.
/*! \fn CIconChooser::CIconChooser(gchar *currentIconFullName, GtkWidget *pwGtkParent)
    \brief CIconChooser constructor

    \param[out] currentIconFullName. The default icon to be load when the Icon Chooser program is invoked.
    \param[out] pwGtkParent. The parent window of the Icon Chooser window.
*/
CIconChooser::CIconChooser(gchar *currentIconFullName, GtkWidget *pwGtkParent)
{
  m_pwParent = NULL;

  m_IconView = NULL;
  m_TreeSelection = NULL;
  m_ListStore = NULL;
	
  m_CurrentIcon = NULL;
  m_IconBrowseLocation = NULL;
	
  m_DefaultIconPath = g_strdup(DEFAULT_ICON_PATH);  
  m_DefaultIcon = g_strdup( DEFAULT_ICON );

  m_bIsChosen = false;

  m_nIconWidth = (gint)ICON_SHOW_WIDTH;
  m_nIconHeight = (gint)ICON_SHOW_HEIGHT;

  m_nButtonWidth = (gint)ICON_BUTTON_WIDTH;
  m_nButtonHeight = (gint)ICON_BUTTON_HEIGHT;

  m_icon_total = 0;
  m_icon_visible_total = 0;

  for(int i=0; i<N_ICONCHOOSER_WIDGET_IDX ;i++)
    m_pWidgets[i] = NULL;  

  /* Set default icon path. */
  #ifdef DEBUG_MENU_ICONCHOOSER
  printf("%s:%s(%d) - Set default icon path [ %s ] \n", __FILE__, __FUNCTION__, __LINE__, currentIconFullName);
  #endif
  if( currentIconFullName == NULL )
     m_CreateInitValue( (gchar*)DEFAULT_ICON );
  else
     m_CreateInitValue(currentIconFullName);

  #ifdef DEBUG_MENU_ICONCHOOSER
  printf("%s:%s(%d) - Prepare UI layout \n", __FILE__, __FUNCTION__, __LINE__);
  #endif
  m_InitLayoutUI(pwGtkParent);
}

/*! \fn CIconChooser::~CIconChooser()
    \brief CIconChooser destructor
*/
CIconChooser::~CIconChooser()
{
  m_pwParent = NULL;
  m_CurrentIcon = NULL;
  m_IconBrowseLocation = NULL;	
  m_DefaultIconPath = NULL;  
  m_DefaultIcon = NULL;  	
  m_bIsChosen = false;

  if(m_CurrentIcon)
    g_free(m_CurrentIcon);

  if(m_IconBrowseLocation)
		g_free(m_IconBrowseLocation);	

  if (m_DefaultIconPath)
		g_free(m_DefaultIconPath);  

  if (m_DefaultIcon)
		g_free(m_DefaultIcon);  
}

/*! \fn void CIconChooser::m_GetWindowSize(int &nWidth, int &nHeight)
    \brief Get the dimension of Icon Chooser window.

    \param[out] nWidth. The width of the Icon Chooser window.
    \param[out] nHeight. The height of the Icon Chooser window.
    \return NONE
*/
void CIconChooser::m_GetWindowSize(int &nWidth, int &nHeight)
{
  nWidth = this->m_nMaxWidth;
  nHeight = this->m_nMaxHeight;

  return;
}

/*! \fn void CIconChooser::m_CreateInitValue(gchar *currentIconFullName)
    \brief Create initial value

    \param[in] currentIconFullName. The initial icon path to load icons shown in the GtkIconView.
    \n If the parameter's value is "NULL", it means using the last assigned icon full name and icon browsing path.
    \return NONE
*/
void CIconChooser::m_CreateInitValue(gchar *currentIconFullName)
{
  /* To set the icon pool path to find candicated icons. */
  if(currentIconFullName)
  {
     /* To store the current used icon's full name. */
     m_SetCurrentIcon(currentIconFullName);
  }
  else
  {
     if(m_IconBrowseLocation)  /* To check if there has assigned current used icon full name. */
       ;  // Do nothing.
     else if (m_DefaultIcon)    /* To check if there has user defined default icon full name. */
       m_SetIconBrowseLocation(m_DefaultIconPath);
     else
       m_SetIconBrowseLocation( (gchar*)DEFAULT_ICON_PATH );
  }

  /* To reset the flag that indicates it there is any application item is chosen. */
  m_bIsChosen = false;

  /* To create a list store(model).
     There has list fields: 
        { Pixel-Buffer, String, String } .
  */
  if(m_ListStore == NULL)
    m_ListStore = gtk_list_store_new(NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
}

/*! \fn gboolean CIconChooser::m_InitLayoutUI(GtkWidget *pwGtkParent)
    \brief Initialize Icon Chooser UI(widgets).

    \param[in] pwGtkParent. The upper level GTK window
    \return TRUE or FALSE
*/
gboolean CIconChooser::m_InitLayoutUI(GtkWidget *pwGtkParent)
{
  char totalIcon[6] = {0}, totalIconVisible[6] = {0};  // Currently, it could only show 5-digit number.

  /* To store the top-level GTK dialog window. */
  if(pwGtkParent != NULL)
    m_pwParent = pwGtkParent;

  m_UpdateIconLocation();

  /*To remove the old tree model. */
  m_RemoveOldTreeModel(false);

  /* To load icon contents through the set icon pool path. */
  m_LoadIconList();

  /* To invoke the icon browsing dialog. */
  InitIconChooserDlg( (MAX_WIDTH/5), (MAX_HEIGHT/5) );

  /* Set the total number of icons. */
  #ifdef DEBUG_MENU_ICONCHOOSER
  printf("%s(%d) - Total number of icon = %d \n", __FUNCTION__, __LINE__, m_icon_total);
  printf("%s(%d) - Total number of Visible icon = %d \n\n", __FUNCTION__, __LINE__, m_icon_visible_total);
  #endif

  sprintf(totalIcon, "%d", m_icon_total);
  sprintf(totalIconVisible, "%d", m_icon_visible_total);
  
  gtk_entry_set_text((GtkEntry*)m_pWidgets[ICONCHOOSER_GtkEntry_IconTotal], (gchar*)totalIcon);
  gtk_entry_set_text((GtkEntry*)m_pWidgets[ICONCHOOSER_GtkEntry_VisibleIconTotal], (gchar*)totalIconVisible);

  return true;
}

/*! \fn void CIconChooser::m_InitIconChooserDlg(int nPosX, int nPosY)
    \brief Initial Icon Chooser UI

    \param[in] nPosX. The X-axis position of Icon Chooser window.
    \param[in] nPosY. The Y-axis position of Icon Chooser window.
    \return    NONE
*/
void CIconChooser::InitIconChooserDlg(int nPosX, int nPosY)
{
  GtkWidget *window = NULL, *scrollWin = NULL;
  GtkWidget *buttonApply = NULL, *buttonClose = NULL;
  GtkWidget *pFixedContainer = NULL;
  GtkWidget *iconView = NULL;
  GtkWidget *textEntry = NULL;
  GtkWidget *textentry_iconTotal = NULL, *textentry_iconVisibleTotal = NULL;
  GtkWidget *label_iconTotal = NULL, *label_iconVisibleTotal = NULL;
#ifdef USE_FILECHOOSER
  GtkWidget *buttonIconPathBrowse = NULL;
#endif

//-------------- Create a main window for the file add/remove dialog.
  /* Create a new window object */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /* Set the title of the window. */
  gtk_window_set_title(GTK_WINDOW(window), WINDOW_TITLE);

  /* Set the size of the chooser window. */
  gtk_widget_set_size_request(window, MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);

  /* Set the window to be irresiable. */
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

  /* If you set the deletable property to FALSE using this function, 
		 GTK+ will do its best to convince the window manager not to show a close button. */
  gtk_window_set_deletable(GTK_WINDOW(window), FALSE);

  /* The event mask of the widget determines, what kind of event will a particular widget receive.
     This set all bit-flags of the Icon Chooser window to receive all events. */
  gtk_widget_add_events(window, GDK_ALL_EVENTS_MASK);		

  /* Set the window's position. */
  gtk_window_move(GTK_WINDOW(window), nPosX, nPosY);

  /* Connect the "destroy" event to a signal handler.
     This event occurs when we call gtk_widget_destroy() on the window,
     or if we return 'FALSE' in the "delete_event" callback. */
  gtk_signal_connect(GTK_OBJECT (window), "destroy",
                     GTK_SIGNAL_FUNC (gtk_main_quit), NULL);	

  /* Store the required widgets ... */
  m_pWidgets[ICONCHOOSER_GtkWindow_Main] = window;

//-------------- Create the gtk fixed container
  /* The main fixed window in main window */
  pFixedContainer = gtk_fixed_new();

  /* Add the main fixed window into the main window */
  gtk_container_add(GTK_CONTAINER (window), pFixedContainer);	

//-------------- Create a scrolled window widget instance.
  /* Create a scroll window object. */
  scrollWin = gtk_scrolled_window_new(NULL, NULL);

  /* Set the scrolls showing policy of the scroll window object. */
  gtk_scrolled_window_set_policy((GtkScrolledWindow*)(scrollWin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  /* Set the shadow type of the scroll window. */
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrollWin), GTK_SHADOW_ETCHED_IN);
	
//-------------- Create a IconView widget instance with list-type model(e.g. data)
  /* Create a icon view object. */
  iconView = m_CreateIconView();

  /* Set the orientation of hte icon view. */
  gtk_icon_view_set_orientation(GTK_ICON_VIEW(iconView), GTK_ORIENTATION_VERTICAL);

  /* Get the icon view selection object from the icon view object. */
  gtk_container_add(GTK_CONTAINER(scrollWin), iconView);

  /* Set the scrolled window's size. */
  gtk_widget_set_size_request(scrollWin, SCROLL_WIDTH, SCROLL_HEIGHT);

  /* Put scrolled window to the fixed windows. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), scrollWin, 10, 40);

  /* Store the required widgets. */
  m_pWidgets[ICONCHOOSER_GtkIconView] = iconView;	

  /* Set the signal connection for GtkIconView object. */
  g_signal_connect(iconView, "selection_changed", G_CALLBACK(cb_selection_changed), this);

//-------------- Create button widget instances
#ifdef USE_FILECHOOSER
  /* The button to invoke a file dialog(FileChooser) for user to choose a folder containing icons. */
  /* Create a button instance. */
  buttonIconPathBrowse = gtk_button_new_with_label( _("Browse...") );  /* For multi-language. */

  /* Set the position(coordinate) in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), buttonIconPathBrowse, 450, 10);

  /* Set the widget's size. */
  gtk_widget_set_size_request(buttonIconPathBrowse, 70, 25);

  /* Store the required widgets. */
  m_pWidgets[ICONCHOOSER_GtkButton_BrowseIcon] = buttonIconPathBrowse;	

  /* Set the signal connection for the "add" button */
  g_signal_connect(GTK_OBJECT(buttonIconPathBrowse), "clicked", G_CALLBACK(on_browse_icon_path), this);
#endif

  // The button is to confirm that it want to use the chosen icon.
  /* Create a button instance. */
  buttonApply = gtk_button_new_with_label( _("Apply") );  /* gtk_button_new_from_stock(GTK_STOCK_APPLY); */  // If you want to use inbuilt button, use gtk_button_new_from_stock() instead.
	
  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), buttonApply, 420, 360);   /* set coordinate. */
	
  /* Set the widget's size. */
  gtk_widget_set_size_request(buttonApply, 80, 30);

  /* Set the signal connection for the "add" button */
  g_signal_connect(GTK_OBJECT(buttonApply), "clicked", G_CALLBACK(on_file_apply), this);

  /* The button to exit the editor. */
  /* Create a button instance. */
  buttonClose = gtk_button_new_from_stock(GTK_STOCK_CLOSE);  /* gtk_button_new_with_label("Close"); */  /* If you want to use multi-language, use button_new_with_label() instead. */
	
  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), buttonClose, 320, 360);   /* set coordinate. */
	
  /* Set the widget's size. */
  gtk_widget_set_size_request(buttonClose, 80, 30);

  /* Set the signal connection for the "Cancel" button.
     This will cause the window to be destroyed by calling
     gtk_widget_destroy(window) when "clicked".  Again, the destroy
     signal could come from here, or the window manager. */
  g_signal_connect(GTK_OBJECT(buttonClose), "clicked", G_CALLBACK(on_close), this);
	
//-------------- Create a text entry widget instances.
  /* Create the text entry widget instance. */
  textEntry = gtk_entry_new();
      
  /* Create a text entry widget instance. */
  //gtk_entry_set_editable((GtkEntry*)textEntry, true);
  gtk_editable_set_editable((GtkEditable*)textEntry, true);

  /* If it had the preset icon full name, show it on the text entry. */
  if(m_CurrentIcon)
    gtk_entry_set_text((GtkEntry*)textEntry, m_CurrentIcon);

  /* Set the size measured in the number of characters. */	 
  gtk_entry_set_width_chars((GtkEntry*)textEntry, (gint)MAX_ICON_PATH); 

  /* Set the widget's size. */
  gtk_widget_set_size_request(textEntry, TEXT_ENTRY_WIDTH, TEXT_ENTRY_HEIGHT);

  /* Put the text entry widget into the file fixed window. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), textEntry, 10, 10);

  /* Store the required widgets. */
  m_pWidgets[ICONCHOOSER_GtkEntry_IconPathName] = textEntry;

  /* Set the signal connection for the "add" button */
  g_signal_connect(GTK_OBJECT(textEntry), "changed", G_CALLBACK(cb_text_changed), this);

//-------------- Create a text entry and label widget instances for showing total number of icons.
  /* Create the label widget instance. */
  label_iconTotal = gtk_label_new( _("Total Num:") );

  /* Set the widget's size. */
 // gtk_widget_set_size_request(label_iconTotal, 80, 30);

  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), label_iconTotal, 15, 350);   /* set coordinate. */

  /* Create the label widget instance. */
  label_iconVisibleTotal = gtk_label_new( _("Total Num(visible):") );

  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), label_iconVisibleTotal, 15, 375);   /* set coordinate. */

  /////////////
  /* Create the text entry widget instance. */
  textentry_iconTotal = gtk_entry_new();
      
  /* Create a text entry widget instance. */
  //gtk_entry_set_editable((GtkEntry*)textentry_iconTotal, true);
  gtk_editable_set_editable((GtkEditable*)textentry_iconTotal, false);

  /* Set the widget's size. */
  gtk_widget_set_size_request(textentry_iconTotal, 60, TEXT_ENTRY_HEIGHT);

  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), textentry_iconTotal, 150, 345);   /* set coordinate. */

  /* Store the required widgets. */
  m_pWidgets[ICONCHOOSER_GtkEntry_IconTotal] = textentry_iconTotal;	

  /* Create the text entry widget instance. */
  textentry_iconVisibleTotal = gtk_entry_new();
      
  /* Create a text entry widget instance. */
  //gtk_entry_set_editable((GtkEntry*)textentry_iconVisibleTotal, true);
  gtk_editable_set_editable((GtkEditable*)textentry_iconVisibleTotal, false);

  /* Set the widget's size. */
  gtk_widget_set_size_request(textentry_iconVisibleTotal, 60, TEXT_ENTRY_HEIGHT);

  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), textentry_iconVisibleTotal, 150, 370);   /* set coordinate. */

  /* Store the required widgets. */
  m_pWidgets[ICONCHOOSER_GtkEntry_VisibleIconTotal] = textentry_iconVisibleTotal;	
}

/*! \fn void CIconChooser::m_DeinitValue(void)
    \brief Close initial value.

    \param[in] NONE
    \return NONE
*/
void CIconChooser::m_DeinitValue(void)
{
  if( m_pWidgets[ICONCHOOSER_GtkIconView] )
    m_RemoveOldTreeModel(true);
}

/*! \fn gboolean CIconChooser::m_DoModal (void)
    \brief Icon Chooser runs in modal mode.

    \param[in] NONE
    \return TRUE or FALSE
*/
gboolean CIconChooser::m_DoModal(void)
{
  /* Sets a window modal or non-modal. */
  gtk_window_set_modal(GTK_WINDOW(m_pWidgets[ICONCHOOSER_GtkWindow_Main]), TRUE);

  if( m_pwParent != NULL )
  {	
     /* Dialog windows should be set transient for the main application window they were */
     /* spawned from. This allows window managers to e.g. keep the dialog on top of the  */
     /* main window, or center the dialog over the main window.                          */
     gtk_window_set_transient_for(GTK_WINDOW(m_pWidgets[ICONCHOOSER_GtkWindow_Main]), GTK_WINDOW(m_pwParent));

     /* Creates the GDK (windowing system) resources associated with a widget. */
     gtk_widget_realize(m_pWidgets[ICONCHOOSER_GtkWindow_Main]);

     /* Sets hints about the window management functions to make available via buttons on the window frame. */
     gdk_window_set_functions(m_pWidgets[ICONCHOOSER_GtkWindow_Main]->window, (GdkWMFunction)(GDK_FUNC_CLOSE|GDK_FUNC_MOVE));
  }

  /* Show all widgets */
  gtk_widget_show_all(m_pWidgets[ICONCHOOSER_GtkWindow_Main]);

  /* Start to run. */
  gtk_main();

  return TRUE;
}

//-------------------------- GtkIconView
/*! \fn GtkTreeModel* CIconChooser::m_CreateAndFillModel(void)
    \brief To create and fill tree model contents.

    \param[in] NONE
    \return GtkTreeModel object.
*/
GtkTreeModel* CIconChooser::m_CreateAndFillModel(void)
{
   return GTK_TREE_MODEL(m_ListStore);
}

/*! \fn GtkWidget*  CIconChooser::m_CreateIconView (void)
    \brief To create a icon view object.

    \param[in] NONE.
    \return The icon view object.
*/
GtkWidget* CIconChooser::m_CreateIconView(void)
{	  
  GtkCellRenderer *rendererText = NULL, *rendererPixBuf = NULL;
  GtkWidget *iconview = NULL;
  GtkTreeModel *model = NULL;

  /* To create a GtkIconView object. */
  iconview = gtk_icon_view_new();

  /* To create a Pixel-Buffer cell-renderer object. */
  rendererPixBuf = gtk_cell_renderer_pixbuf_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconview), rendererPixBuf, FALSE);	
  g_object_set(rendererPixBuf, "follow-state", TRUE, NULL);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(iconview), rendererPixBuf, "pixbuf", 0, NULL);

  /* To create a Text cell-renderer object. */
  rendererText = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconview), rendererText, FALSE);
  g_object_set(rendererText,
                "editable", FALSE,
                "xalign", 0.5,
                "wrap-mode", PANGO_WRAP_WORD_CHAR,
                "wrap-width", 100,
                NULL);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT (iconview), rendererText, "text", 1, NULL);

  /* The third field of the list-store is invisible to the user. */

  /* To create a list model. */
  model = m_CreateAndFillModel();
  gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), model);
  g_object_unref(model);

  return iconview;
}

/*! \fn GdkPixbuf* CIconChooser::m_LoadIcon(const gchar* name, gint size, gboolean use_fallback )
    \brief To load a icon's image contents.

    \param[in] name.
    \param[in] size. 
    \param[in] use_fallback.
    \return GdkPixbuf object for the icon.
*/
GdkPixbuf* CIconChooser::m_LoadIcon(const gchar* name, gint size, gboolean use_fallback)
{
  GtkIconTheme *theme = NULL;
  gchar *icon_name = NULL, *suffix = NULL;
  GdkPixbuf *icon = NULL;

  if( name )
  {
     if( g_path_is_absolute(name) )
        icon = gdk_pixbuf_new_from_file_at_size(name, size, size, NULL);
     else
     {
        theme = gtk_icon_theme_get_default();
        suffix = strchr( (char*)name, '.' );
			
        if( suffix )   /* has file extension, it's a basename of icon file */
        {
		   /* Try to find it in "pixmaps", "icons/hicolor", "icons/hicolor/scalable/apps" directories */
		   icon = m_LoadIconFile( name, size );

           if( G_UNLIKELY(!icon) )  /* unfortunately, not found */
           {
              /* Let's remove the suffix, and see if this name
				 can match an icon in current icon theme */
              icon_name = g_strndup( name, (suffix - name) );
              icon = m_LoadThemeIcon( theme, icon_name, size );
              g_free( icon_name );
           }
         }
         else  /* no file extension, it could be an icon name in the icon theme */
           icon = m_LoadThemeIcon( theme, name, size );
     }
  }

  if( G_UNLIKELY(!icon) && use_fallback )  /* fallback to generic icon */
  {
     theme = gtk_icon_theme_get_default();
     icon = m_LoadThemeIcon( theme, DEFAULT_APP_ICON, size );

     if( G_UNLIKELY(!icon) )  /* fallback to generic icon */
       icon = m_LoadThemeIcon( theme, DEFAULT_APP_MIME_ICON, size );
  }

  return icon;
}

/*! \fn GdkPixbuf* CIconChooser::m_LoadIconFile(const char* file_name, int size)
    \brief Try to find it in "pixmaps", "icons/hicolor", "icons/hicolor/scalable/apps" directories.

    \param[in] file_name.
    \param[in] size.
    \return GdkPixbuf object for the icon.
*/
GdkPixbuf* CIconChooser::m_LoadIconFile(const char* file_name, int size)
{
  GdkPixbuf *icon = NULL;
  char *file_path = NULL;
  const gchar **dirs = (const gchar**)g_get_system_data_dirs();
  const gchar **dir = NULL;
  gchar *sizeName = NULL; 

  for( dir = dirs; *dir; ++dir )
  {
     /* Searching in "/usr/share/pixmaps" directory. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free(file_path);

     file_path = NULL;
		
     if( icon )
       break;

     /* Searching icon in "/usr/share/icons/hicolor/SizexSize/apps", 
        "/usr/local/share/icons/hicolor/SizexSize/apps" directories.
        These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_HICOLOR, size, size );
     file_path = g_build_filename( *dir, sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     if(sizeName)
       g_free(sizeName);

     file_path = NULL;
     sizeName = NULL;

     if( icon )
       break;

     /* Searching in "/usr/share/icons/hicolor/scalable/apps",  "/usr/local/share/icons/hicolor/scalable/apps". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_HICOLOR_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     file_path = NULL;

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/scalable". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     file_path = NULL;	

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/scalable/apps". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE_APPS, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     file_path = NULL;

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/SizexSize/apps".
        These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_GNOME, size, size );
     file_path = g_build_filename( *dir,  sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free(file_path);

     if(sizeName)
       g_free(sizeName);

     file_path = NULL;
     sizeName = NULL;

     if( icon )
       break;
  }

  return icon;
}

/*! \fn GdkPixbuf* CIconChooser::m_LoadThemeIcon(const char* file_name, int size)
    \brief To load a icon contents found in theme icon pool.

    \param[in] theme.
    \param[in] icon_name. 
    \param[in] size.
    \return GdkPixbuf object for the icon.
*/
GdkPixbuf* CIconChooser::m_LoadThemeIcon(GtkIconTheme* theme, const char* icon_name, int size)
{
  GdkPixbuf *icon = NULL;
  const char *file = NULL;
  GtkIconInfo *info = gtk_icon_theme_lookup_icon( theme, icon_name, size,  GTK_ICON_LOOKUP_USE_BUILTIN );

  if( G_UNLIKELY( !info ) )
    return NULL;

  file = gtk_icon_info_get_filename( info );

  if( G_LIKELY( file ) )
    icon = gdk_pixbuf_new_from_file( file, NULL );
  else
    icon = gtk_icon_info_get_builtin_pixbuf( info );

  gtk_icon_info_free( info );

  if( G_LIKELY(icon) )   /* scale down the icon if it's too big */
  {
    int	height = gdk_pixbuf_get_height(icon);
    int	width = gdk_pixbuf_get_width(icon);

    if( G_UNLIKELY( (height > size) || (width > size) ) )
    {
       GdkPixbuf *scaled = NULL;

       if( height > width )
       {
          width = size * height / width;
          height = size;
       }
       else if( height < width )
       {
          height = size * width / height;
          width = size;
       }
       else
          height = width = size;

       scaled = gdk_pixbuf_scale_simple( icon, width, height, GDK_INTERP_BILINEAR );
       g_object_unref( icon );
       icon = scaled;
     }
  }

  return icon;
}

/*! \fn gboolean CIconChooser::m_LoadIconList(void)
    \brief To load a icons' content and append contents to the list-store.

    \param[in] NONE.
    \return TRUE or FALSE
*/
gboolean CIconChooser::m_LoadIconList(void)
{
  GDir *pDir = NULL;
  GError *errOpen = NULL;
  gchar *baseName = NULL, *fullName = NULL;   /* To store the icon file's basename, e.g. filename.extension, and full name, e.g. path/filename.extension. */
  GtkTreeIter iter;

  /* To check if it had been assigned a directory name. */
  if( m_IconBrowseLocation == NULL )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf ("\n\n %s(%d) Error! The icons browsing location path is NULL! \n\n", __FUNCTION__, __LINE__);
     #endif

     return false;
  }

  /* To set the if the the current file name is a directory. */
  if( g_file_test(m_IconBrowseLocation,(GFileTest) G_FILE_TEST_IS_DIR) == false )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf ("\n\n %s(%d) Error! The icons browsing location path is not a directory! \n\n", __FUNCTION__, __LINE__);
     #endif

     return false;
  }

  /* To open the icon browsing directory. */
  pDir = g_dir_open(m_IconBrowseLocation, 0, &errOpen);
  if(pDir == NULL)
  {
     if(errOpen)
     {
        #ifdef DEBUG_MENU_ICONCHOOSER
        printf ("\n\n %s(%d) Error! %s! \n\n", __FUNCTION__, __LINE__, errOpen->message);
        #endif

        g_error_free(errOpen);
     }

     return false;
  }		

  /* To retrieve the file name of icons in the chosen directory irrecusively. */
  while( (baseName = (gchar*)g_dir_read_name(pDir)) != NULL )
  {	
     GdkPixbuf *pixbuf = NULL;

     fullName = g_strdup_printf("%s%s", m_IconBrowseLocation, baseName);

     /* To check if the the currently read icon file name is valid. */
     if( m_IsPhotoFile((gchar *)baseName) == FALSE )
       continue;

     /* Increae the counter for read icon with valid file name. */
     m_icon_total++;

     /* To retrieve the basename of the icon file. */
     baseName = g_path_get_basename(fullName);

     /* To create the icon for the currently read node. */
     pixbuf = m_LoadIcon(fullName, IMG_SIZE, NULL);			

     /* To check if it had load image successfully. */
     if( pixbuf )
     {
        /* Append a row and fill in some data */
        /* Step 1) To request a new node memory to be add new data. */
        gtk_list_store_append(m_ListStore, &iter);

        /* Step 2) To put the text into the allocated memory .The list is terminated by a -1.  */
        gtk_list_store_set(m_ListStore, &iter,
                           COLUMN_ICON, pixbuf,
                           COLUMN_ICONNAME, baseName,
                           COLUMN_ICONPATH, fullName,
                           -1);	
			
        /* The PixBuf has a refcount of 1st now, as the list store has added its own reference */
        g_object_unref(pixbuf);

        /* Increae the counter for visible icon(e.g. could be shown in icon view). */
        m_icon_visible_total++;
     }

     /* To free the allocated memory for storing the icon full name. */
     if(fullName)
       g_free(fullName);

     if(baseName)
       g_free(baseName);
  }	

  return true;
}

/*! \fn gboolean CIconChooser::m_ReloadIconList(gchar *iconpath)
    \brief To reload a icon contents and append contents to the list-store.

    \param[in] iconpath.
    \return TRUE or FALSE
*/
gboolean CIconChooser::m_ReloadIconList(gchar *iconpath)
{ 
  /* First, to clear out the list-store's contents. */
  GtkTreeModel *model = NULL;
  char totalIcon[6] = {0}, totalIconVisible[6] = {0};  // Currently, it could only show 5-digit number.

  m_bIsChosen = false;
  m_RemoveOldTreeModel(false);

  /* Second, to load icons' content into the list-store. */
  if(iconpath)
    m_SetIconBrowseLocation(iconpath);  /* To set the icon browsing path. */

  /* To call the funciton to build the list-store contents.
     The path to icons may be changed when it is set current icon full name. */
  m_LoadIconList();

  /* Finally, to set the tree model to the icon view. */
  model = m_CreateAndFillModel();
  gtk_icon_view_set_model(GTK_ICON_VIEW(m_pWidgets[ICONCHOOSER_GtkIconView]), model);
  g_object_unref(model);

  /* Set the total number of icons. */
  #ifdef DEBUG_MENU_ICONCHOOSER
  printf("%s(%d)  Total number of icon = %d \n", __FUNCTION__, __LINE__, m_icon_total);
  printf("%s(%d)  Total number of Visible icon = %d \n\n", __FUNCTION__, __LINE__, m_icon_visible_total);
  #endif

  sprintf(totalIcon, "%d", m_icon_total);
  sprintf(totalIconVisible, "%d", m_icon_visible_total);
  
  gtk_entry_set_text((GtkEntry*)m_pWidgets[ICONCHOOSER_GtkEntry_IconTotal], (gchar*)totalIcon);
  gtk_entry_set_text((GtkEntry*)m_pWidgets[ICONCHOOSER_GtkEntry_VisibleIconTotal], (gchar*)totalIconVisible);

  return true;
}

/*! \fn	gboolean CIconChooser::m_IsPhotoFile(gchar *pFile)
    \brief To determine if the current read image format is valid.

    \param[in] pFile. The icon file name.
    \return TRUE or FALSE
*/
gboolean CIconChooser::m_IsPhotoFile(gchar *pFile)
{
  gboolean bRet = FALSE;
  gchar *pStr;

  pStr = g_ascii_strdown(pFile, -1);

  bRet = g_str_has_suffix (pStr,".jpg");
  if(bRet == TRUE)
  {
    g_free(pStr);
    return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".jpeg");
  if(bRet==TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".jpe");
  if(bRet==TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".png");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".gif");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".tif");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".tiff");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".bmp");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".svg");
  if(bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  bRet = g_str_has_suffix(pStr,".xpm");
  if (bRet == TRUE)
  {
     g_free(pStr);
     return TRUE;
  }

  g_free(pStr);

  return FALSE;
}

/*! \fn	void CIconChooser::m_SetCurrentIcon(gchar *icon)
    \brief To set the current chosen icon's full name string.

    \param[in] icon. The current chosen icon's full path.
    \return NONE
*/
void CIconChooser::m_SetCurrentIcon(gchar *icon)
{
  if( (icon == NULL) || (strlen(icon) == 0) )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf("\n\n %s(%d)  Error! The passed-in icons full name is NULL! \n\n", __FUNCTION__, __LINE__);
     #endif

     return;
  }
	
  if(m_CurrentIcon)
  {
     g_free(m_CurrentIcon);
     m_CurrentIcon = NULL;
  }	

  m_CurrentIcon = g_strdup(icon);

  /* To update the icon browing location. */
  m_UpdateIconLocation();
} 

/*! \fn	void CIconChooser::m_SetCurrentIcon(gchar *iconlocation)
    \brief To set the current chosen icons locating path string.

    \param[in] iconlocation. The current chosen icon's full path name.
    \return NONE
*/
void CIconChooser::m_SetIconBrowseLocation(gchar *iconlocation) 
{
  if( (iconlocation == NULL) || (strlen(iconlocation) == 0) )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf("\n\n %s(%d)  Error! The passed-in icons locating path is NULL! \n\n", __FUNCTION__, __LINE__);
     #endif

     return;
  }

  if(m_IconBrowseLocation)
  {
     g_free(m_IconBrowseLocation);
     m_IconBrowseLocation = NULL;
  }

  m_IconBrowseLocation = g_strdup(iconlocation);
} 

/*! \fn	void CIconChooser::m_SetDefaultIconPath(gchar *defaultIconPath)
    \brief To set the default icons searching path.

    \param[in] defaultIconPath. The default icons path.
    \return NONE
*/
void CIconChooser::m_SetDefaultIconPath(gchar *defaultIconPath)
{
  if( (defaultIconPath==NULL) || (strlen(defaultIconPath) == 0) )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
	 printf("\n\n %s(%d)  Error! The passed-in default icons searching path is NULL! \n\n", __FUNCTION__, __LINE__);
     #endif

     return;
  }

  if(m_DefaultIconPath)
  {
     g_free(m_DefaultIconPath);
     m_DefaultIconPath = NULL;
  }

  m_DefaultIconPath = g_strdup(defaultIconPath);
} 

/*! \fn void CIconChooser::m_SetDefaultIcon(gchar *defaultIcon)
    \brief To set the default icon full name.

    \param[in] defaultIcon. The default icon full name.
    \return NONE
*/
void CIconChooser::m_SetDefaultIcon(gchar *defaultIcon)
{
  if( (defaultIcon == NULL) || (strlen(defaultIcon) == 0) )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf("\n\n %s(%d)  Error! The passed-in default icon full name is NULL! \n\n", __FUNCTION__, __LINE__);
     #endif

     return;
  }

  if(m_DefaultIcon)
  {
     g_free(m_DefaultIcon);
     m_DefaultIcon = NULL;
  }

  m_DefaultIcon = g_strdup(defaultIcon);
} 

/*! \fn gchar* CIconChooser::m_GetIconFullName(const char* file_name, int size)
    \brief Try to find it in "pixmaps", "icons/hicolor", "icons/hicolor/scalable/apps" dirs.

    \param[in]  file_name.
    \param[in]  size. 
    \return The string representing the icon's full name.
*/
gchar* CIconChooser::m_GetIconFullName(const char* file_name, int size)
{
  GdkPixbuf *icon = NULL;
  char *file_path = NULL;
  const gchar **dirs = (const gchar**)g_get_system_data_dirs();  /* To read the setting of the environment variable : "XDG_DATA_DIRS"*/
  const gchar **dir = NULL;
  gchar *sizeName = NULL;

  for( dir = dirs; *dir; ++dir )
  {
     /* Searching in "/usr/share/pixmaps" directory. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the list store has added its own reference.
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)
          g_free(file_path);

        file_path = NULL;
     }

     /* Searching in "/usr/share/icons/hicolor/SizexSize/apps",
        "/usr/local/share/icons/hicolor/SizexSize/apps" directories.
		These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_HICOLOR, size, size );
     file_path = g_build_filename( *dir, sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(sizeName)
       g_free(sizeName);

     sizeName = NULL;

     if ( icon )
     {
        /* pixbuf has a referece count of "1" now, as the list store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)
          g_free( file_path );

        file_path = NULL;
     }

     /* Searching in "/usr/share/icons/hicolor/scalable/apps",  "/usr/local/share/icons/hicolor/scalable/apps". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_HICOLOR_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

    if( icon )
    {
       /* pixbuf has a referece count of "1" now, as the list store has added its own reference. 
          So to decrease the reference count of pixbuf. */
       g_object_unref(icon);
       break;
    }
    else
    {
       if(file_path)
         g_free( file_path );

       file_path = NULL;			
    }

    /* Searching in "/usr/share/icons/gnome/scalable". */
    file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE, file_name, NULL );
    icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

    if( icon )
    {
       /* pixbuf has a referece count of "1" now, as the list store has added its own reference. 
          So to decrease the reference count of pixbuf. */
       g_object_unref(icon);
       break;
    }
    else
    {
       if(file_path)
         g_free( file_path );

       file_path = NULL;
    }

    /* Searching in "/usr/share/icons/gnome/scalable/apps" directory. */
    file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE_APPS, file_name, NULL );
    icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

    if( icon )
    {
       /* pixbuf has a referece count of "1" now, as the list store has added its own reference. 
          So to decrease the reference count of pixbuf. */
       g_object_unref(icon);
       break;
    }
    else
    {
       if(file_path)
         g_free( file_path );

       file_path = NULL;
    }

    /* Searching in "/usr/share/icons/gnome/SizexSize/apps".
       These are needed to assign the directory name formed in size. */
    sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_GNOME, size, size );
    file_path = g_build_filename( *dir,  sizeName, file_name, NULL );
    icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

    if(sizeName)
      g_free(sizeName);

    sizeName = NULL;

    if( icon )
    {
       /* pixbuf has a referece count of "1" now, as the list store has added its own reference. 
          So to decrease the reference count of pixbuf. */
       g_object_unref(icon);			
       break;
    }
    else
    {
       if(file_path)
         g_free (file_path);

       file_path = NULL;
    }
  }

  return  file_path;
}

/*! \fn void CIconChooser::m_UpdateIconLocation(void)
    \brief To update the icon browsing locaiton.

    \param[in] NONE.
    \return NONE.
*/
void CIconChooser::m_UpdateIconLocation(void)
{
  gchar *dirName = NULL;

  if( !m_CurrentIcon )
  {
     #ifdef DEBUG_MENU_ICONCHOOSER
     printf("\n Look! There has no setting to current used icon name. \n");
     #endif

     return;
  }

  /* To get the directory name of the current used icon. */
  dirName = g_path_get_dirname(m_CurrentIcon);
  if(dirName)
  {
     if( g_str_has_suffix(dirName, "/") == false )
     {
        gint len = strlen(dirName) + 1 + 1;   // one for a "/" and one for a NULL character.
        gchar *tmpStr = (gchar*)malloc( sizeof(gchar) * len );

        if(tmpStr)
        {
           memset (tmpStr, 0x00, len);
           tmpStr = g_strdup_printf("%s%s", dirName, "/");
           m_SetIconBrowseLocation(tmpStr);						
           g_free(tmpStr);
        }
     }	
     else
       m_SetIconBrowseLocation(dirName);

     g_free(dirName);
  }
}

/*! \fn void CIconChooser::m_RemoveOldTreeModel(gboolean isDeinit)
    \brief To remove the old tree model.

    \param[in] isDeinit. To determine if it need to do deinitialization operation.
    \return NONE.
*/
void CIconChooser::m_RemoveOldTreeModel(gboolean isDeinit)
{
  if( m_pWidgets[ICONCHOOSER_GtkIconView] )
  {
     if( m_ListStore )
     {
        GtkTreeModel *model = NULL;
        GtkTreeIter iter;

        /* Get the model. */
        model = gtk_icon_view_get_model( GTK_ICON_VIEW(m_pWidgets[ICONCHOOSER_GtkIconView]) );
        m_ListStore = GTK_LIST_STORE(model);

        /* Make sure the model stays with us after the icon view unrefs it */
        g_object_ref( (GObject*)m_ListStore ); 	

        /* Detach the model from the view */
        gtk_icon_view_set_model(GTK_ICON_VIEW(m_pWidgets[ICONCHOOSER_GtkIconView]), NULL);

        /* Initializes iterator with the first iterator in the list(tree) (the one at the path "0") and returns TRUE. */	
        if( gtk_tree_model_get_iter_first(model, &iter) == TRUE )
        {
           /* To clear all tree model data. */
           gtk_list_store_clear(m_ListStore);			
           m_ListStore = NULL;
        }
     }

     /* Reset the counter for the amount of icons. */
     m_icon_total = 0;
     m_icon_visible_total = 0;

     /* To re-create a list-store model.
        There has tree fields in type respectively : 
           { Pixel-Buffer, String, String }
     */
     if(isDeinit == false)
       m_ListStore = gtk_list_store_new(NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
  }
}

