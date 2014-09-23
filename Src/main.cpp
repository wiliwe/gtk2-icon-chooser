/*! \file    main.cpp
    \brief   To demostrate how to use Icon Chooser tool..

    \author  William.L    
    \datee   2008-09-26
    \version 1.0

    \b Change_History: 
    \n 1) 2008-09-26 William.L initial version. 
*/

#include <stdio.h>
#include <glib/gi18n.h>   // For GNU gettext i18n, multi-language

#include "CIconChooser.h"

//#define TEST

#ifdef TEST
/* For GNU gettext i18n, multi-language */
#define PACKAGE   "IconChooser"
#define LOCALEDIR "./locale"

int main(int argc, char* argv[])
{
  /* For GNU gettext i18n, multi-language */
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  /* First of all, call gtk_init() to initialize GTK type system.

     If you do not call this first of all GTK codes,
     you will get errror message : 
       "You forgot to call g_type_init()"
  */
  gtk_init (&argc, &argv);

  CIconChooser iconChooser((gchar*)DEFAULT_ICON_PATH, NULL);

  printf("Start to show dialog \n");

  if( iconChooser.m_DoModal() ) 
  {
    printf("Icon Chooser application terminated! \n\n");
  }
		
  /*!< To set Name, Exec and Comment fields of the dialog. */
  if (iconChooser.m_GetIsChosen())
  {
    if (iconChooser.m_GetCurrentIcon())
       printf("The Selected Icon Name : %s \n", (char*)iconChooser.m_GetCurrentIcon());		
  }

  printf("Going to shutdown \n\n");

  return 0;
}
#endif
