#include <csTypes.h>
#include "csStrUtils.h"

CSAPP_STRING_DATA::CSAPP_STRING_DATA()
{
    originalText = 0;
    viewedText = 0;
    translationContext = 0;
    guideExpression = 0;
    translatedText = 0;
}

bool CSAPP_STRING_DATA::setText(wchar_t** str, wchar_t* _translationContext, wchar_t* _guideExpression)
{
    if(str && *str)
    {
        if(originalText) 
        {
            free(originalText);
            originalText = 0;
        }
        originalText = CSSTRUTILS::makeWString(*str);

        viewedText = str;
        if(_translationContext)
        {
            if(translationContext) { free(translationContext); translationContext = 0;}
            translationContext = CSSTRUTILS::makeWString(_translationContext);
        }
        if(_guideExpression)
        {
            if(guideExpression) { free(guideExpression); guideExpression = 0;}
            guideExpression = CSSTRUTILS::makeWString(_guideExpression);
        }
        return 1;
    }
    return 0;
}

bool CSAPP_STRING_DATA::clear()
{
    free(originalText);
    free(translationContext);
    free(guideExpression);
    free(translatedText);
}

void CSAPP_STRINGS::newString(wchar_t** str, wchar_t* _translationContext, wchar_t* _guideExpression)
{
    CSAPP_STRING_DATA sd;
    sd.setText(str, _translationContext, _guideExpression);
    asd.push_back(sd);
}

bool CSAPP_STRINGS::removeString(wchar_t** str)
{
    int n = asd.size();
    for(int i=0; i<n; i++)
    {
        if(asd[i].viewedText == str)
        {
            asd[i].clear();
            asd.erase(asd.begin()+i);
            return 1;
        }
    }
    return 0;
}
