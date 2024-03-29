/*
 * HandlePages.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */
#include "Application/Visual/Pages/HandlePages.h"

#include "Application/Visual/Pages/PagesTypes.h"
#include "Application/Visual/Pages/Page1.h"
#include "Application/Visual/Pages/Page2.h"

static PagesTypes_t pagesStatus(PagesTypes_t page);
static void showPage(PagesTypes_t page);
static void pageExecute(PagesTypes_t page);

void HandlePages_init(void)
{
    Page1_showPage();
}

void HandlePages_execute(void)
{
    static PagesTypes_t actualPage = PAGES_NUMBER_1;

    PagesTypes_t newPage = pagesStatus(actualPage);
    if (actualPage != newPage)
    {
        actualPage = newPage;
        showPage(actualPage);
    }
    pageExecute(actualPage);
}

/*******************************************
Static functions
*******************************************/

static PagesTypes_t pagesStatus(PagesTypes_t page)
{
    PagesTypes_t retPage = PAGES_NUMBER_1;
    switch(page)
    {
    case PAGES_NUMBER_1:
        retPage = Page1_nextPage();
        break;
    case PAGES_NUMBER_2:
        retPage = Page2_nextPage();
        break;
    default:
      //assert();
      break;
    }
    return retPage;
}

static void showPage(PagesTypes_t page)
{
    switch(page)
    {
    case PAGES_NUMBER_1:
        Page1_showPage();
        break;
    case PAGES_NUMBER_2:
        Page2_showPage();
        break;
    default:
      //assert();
      break;
    }
}

static void pageExecute(PagesTypes_t page)
{
    switch(page)
    {
    case PAGES_NUMBER_1:
        Page1_execute();
        break;
    case PAGES_NUMBER_2:
        Page2_execute();
        break;
    default:
      //assert();
      break;
    }
}

