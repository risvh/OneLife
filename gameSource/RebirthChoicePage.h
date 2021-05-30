#include "GamePage.h"



#include "minorGems/ui/event/ActionListener.h"


#include "TextButton.h"
#include "Background.h"


class RebirthChoicePage : public GamePage, public ActionListener {
        
    public:
        RebirthChoicePage();
        

        void showReviewButton( char inShow );


        virtual void actionPerformed( GUIComponent *inTarget );

        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );

        virtual void makeActive( char inFresh );
		
		// LunarMod
		virtual void keyDown( unsigned char inASCII );
        
    protected:
    
        Background mBackground;
        
        TextButton mQuitButton;
        TextButton mReviewButton;
        TextButton mRebornButton;
        TextButton mGenesButton;
        
        TextButton mTutorialButton;
        TextButton mSettingsButton;
        TextButton mMenuButton;

    };
