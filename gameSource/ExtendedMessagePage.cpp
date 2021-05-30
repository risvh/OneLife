#include "ExtendedMessagePage.h"

#include "buttonStyle.h"
#include "message.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


#include "minorGems/util/SettingsManager.h" //LunarMod


extern Font *oldMainFont;


extern char *userEmail;
extern char *accountKey;





ExtendedMessagePage::ExtendedMessagePage()
          // using the old font here for the black background
        : mOKButton( oldMainFont, 0, -128, 
                     translate( "okay" ) ),
          mMessageKey( "" ),
          mSubMessage( NULL ) {

    addComponent( &mOKButton );
    
    setDarkButtonStyle( &mOKButton );
    
    mOKButton.addActionListener( this );
    }


ExtendedMessagePage::~ExtendedMessagePage() {
    if( mSubMessage != NULL ) {
        delete [] mSubMessage;
        }
    }




void ExtendedMessagePage::setMessageKey( const char *inMessageKey ) {
    mMessageKey = inMessageKey;
    }

void ExtendedMessagePage::setSubMessage( const char *inMessage ) {
    if( mSubMessage != NULL ) {
        delete [] mSubMessage;
        }
    mSubMessage = stringDuplicate( inMessage );
    }



        
void ExtendedMessagePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mOKButton ) {
        setSignal( "done" );
        }
    }



void ExtendedMessagePage::draw( doublePair inViewCenter, 
                                  double inViewSize ) {
    
    doublePair pos = { 0, 200 };
    
    drawMessage( mMessageKey, pos, false, 1.0, true );
    
    if( mSubMessage != NULL ) {
        pos.y = 50;
        drawMessage( mSubMessage, pos, false, 1.0, true );
        }
    
	//LunarMod
    int autoRebirth = SettingsManager::getIntSetting( "autoRebirth", 0 );
	if (autoRebirth == 1) setSignal( "done" );
    }

// LunarMod
void ExtendedMessagePage::keyDown( unsigned char inASCII ) {

    if( inASCII == 10 || inASCII == 13 ) {
        // enter key
		
		setSignal( "done" );


    }
}