#include "Picker.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/game/game.h"




#define PER_PAGE 5

extern Font *mainFont;
extern Font *smallFont;


Picker::Picker( Pickable *inPickable, double inX, double inY )
        : PageComponent( inX, inY ),
          mPressStartedHere( false ),
          mPickable( inPickable ),
          mSkip( 0 ),
          mResults( NULL ),
          mNumResults( 0 ),
          mResultsUnclickable( NULL ),
          mNextButton( smallFont, +60, -290, "Next" ), 
          mPrevButton( smallFont, -60, -290, "Prev" ), 
          mDelButton( smallFont, 15, -290, "x" ), 
          mDelConfirmButton( smallFont, -15, -290, "!" ), 
          mSearchField( mainFont, 
                        0,  100, 4,
                        true,
                        "", NULL, "" ),
          mSelectionIndex( -1 ),
          mSelectionRightClicked( false ),
          mPastSearchCurrentIndex( -1 ) {

    addComponent( &mNextButton );
    addComponent( &mPrevButton );
    addComponent( &mSearchField );
    
    addComponent( &mDelButton );
    addComponent( &mDelConfirmButton );
    

    mNextButton.addActionListener( this );
    mPrevButton.addActionListener( this );
    mSearchField.addActionListener( this );
    
    mDelButton.addActionListener( this );
    mDelConfirmButton.addActionListener( this );
    
    mDelButton.setVisible( false );
    mDelConfirmButton.setVisible( false );

    mSearchField.setFireOnAnyTextChange( true );

    redoSearch( false );

    if( ! mPickable->isSearchable() ) {
        mSearchField.setVisible( false );
        }
    }



Picker::~Picker() {
    if( mResults != NULL ) {
        delete [] mResults;
        delete [] mResultsUnclickable;
        }
    mPastSearches.deallocateStringElements();
    mPastSearchSkips.deleteAll();
    }



void Picker::redoSearch( char inClearPageSkip ) {
    if( inClearPageSkip ) {
        mSkip = 0;
        }
    
    
    char *search = mSearchField.getText();
    
    if( mResults != NULL ) {
        delete [] mResults;
        delete [] mResultsUnclickable;
        mResults = NULL;
        mResultsUnclickable = NULL;
        }

    int numRemain;


    char multiTermDone = false;
    
    if( strstr( search, " " ) != NULL || 
        (strstr( search, "." ) != NULL && strcmp( search, "." ) != 0) ) {
        
        // special case, multi-term search
        
        char *searchLower = stringToLowerCase( search );

        int numTerms;
        char **terms = split( searchLower, " ", &numTerms );

        char *exactQuery = NULL;
        int queryLen = strlen( searchLower );
        if( queryLen > 0 && searchLower[queryLen-1] == '.' ) {
            exactQuery = stringDuplicate( searchLower );
            exactQuery[queryLen-1] = '\0';
            }
        
        delete [] searchLower;

        SimpleVector<char*> validTerms;
        
        // any term that starts with - is a term to avoid
        SimpleVector<char*> avoidTerms;

        // any term that ends with . is a term to exact-match
        SimpleVector<char*> exactTerms;
        
        for( int i=0; i<numTerms; i++ ) {
            int termLen = strlen( terms[i] );
            
            if( termLen > 0 ) {
                
                if( terms[i][0] == '-' ) {
                    if( termLen > 1 ) {
                        // skip the - character
                        avoidTerms.push_back( &( terms[i][1] ) );
                        }
                    // ignore single - characters
                    // user is probably in the middle of typing an avoid-term
                    }
                else {

                    if( termLen > 1 && terms[i][termLen-1] == '.' ) {
                        terms[i][termLen-1] = '\0';
                        exactTerms.push_back( terms[i] );
                        }

                    validTerms.push_back( terms[i] );
                    }
                }
            }
        
        if( validTerms.size() > 0 ) {
            multiTermDone = true;
            
            // do full search for first term
            
            int numMainResults, numMainRemain;
            void **mainResults = 
                mPickable->search( validTerms.getElementDirect( 0 ),
                                   0,
                                   // limit of 1 million
                                   1000000,
                                   &numMainResults, &numMainRemain );
            if( numMainResults == 0 ) {
                mResults = mainResults;
                mNumResults = numMainResults;
                numRemain = numMainRemain;
                }
            else {
                // at least one main result
                
                SimpleVector<void*> passingResults;
                
                for( int i=0; i<numMainResults; i++ ) {
                    const char *mainResultName = 
                        mPickable->getText( mainResults[i] );
                
                    char *mainNameLower = stringToLowerCase( mainResultName );

                    // int numTermsInName;
                    // char **termsInName = split( mainNameLower, " ", &numTermsInName );
                    
                    
                    char matchFailed = false;

                    if( exactQuery != NULL &&
                        strcmp( exactQuery, mainNameLower ) != 0
                        ) {
                        matchFailed = true;
                        }

                    // for( int j=0; j<exactTerms.size(); j++ ) {
                    //     char *term = exactTerms.getElementDirect( j );

                    //     char exactMatched = false;
                    //     for( int k=0; k<numTermsInName; k++ ) {
                    //         int termLen = strlen( termsInName[k] );
                    //         if( termLen > 0 ) {
                    //             if( strcmp( term, termsInName[k] ) == 0 ) {
                    //                 exactMatched = true;
                    //                 break;
                    //                 }
                    //             }
                    //         }
                    //     if( !exactMatched ) {
                    //         matchFailed = true;
                    //         break;
                    //         }

                    //     }
                    
                    if( ! matchFailed ) {
                        for( int j=1; j<validTerms.size(); j++ ) {
                            char *term = validTerms.getElementDirect( j );
                            
                            if( strstr( mainNameLower, term ) == NULL ) {
                                matchFailed = true;
                                break;
                                }
                            }
                        }

                    if( ! matchFailed ) {
                        for( int j=0; j<avoidTerms.size(); j++ ) {
                            char *term = avoidTerms.getElementDirect( j );
                        
                            if( strstr( mainNameLower, term ) != NULL ) {
                                matchFailed = true;
                                break;
                                }
                            }
                        }
                    
                    if( !matchFailed ) {
                        passingResults.push_back( mainResults[i] );
                        }

                    delete [] mainNameLower;
                    }
                
                
                int totalNumResults = passingResults.size();

                mNumResults = totalNumResults -= mSkip;
                numRemain = 0;
                
                if( mNumResults > PER_PAGE ) {
                    numRemain = mNumResults - PER_PAGE;
                    mNumResults = PER_PAGE;
                    }
                
                mResults = new void*[ mNumResults ];

                int resultI = 0;
                for( int j=mSkip; j<mSkip + mNumResults; j++ ) {
                    mResults[resultI] = passingResults.getElementDirect(j);
                    resultI++;
                    }
                
                delete [] mainResults;
                }
            
            }
        
        for( int i=0; i<numTerms; i++ ) {
            delete [] terms[i];
            }
        delete [] terms;

        if( exactQuery != NULL ) delete [] exactQuery;
        }

    if( !multiTermDone ) {
        
        mResults = mPickable->search( search, 
                                      mSkip, 
                                      PER_PAGE, 
                                      &mNumResults, &numRemain );
        }
    
    
    mResultsUnclickable = new char[ mNumResults ];

    for( int i=0; i<mNumResults; i++ ) {
        mResultsUnclickable[i] = false;
        
        for( int u=0; u<mFilterFuncions.size(); u++ ) {
            char unclickable =
                mFilterFuncions.
                getElementDirect( u ).unclickableFunc( 
                    mPickable->getID( mResults[ i ] ) );
                        
            if( unclickable ) {
                mResultsUnclickable[ i ] = true;
                break;
                }
            }
        }


    
    mPrevButton.setVisible( mSkip > 0 );
    mNextButton.setVisible( numRemain > 0 );
    
    delete [] search;

    int oldSelection = mSelectionIndex;
    mSelectionIndex = -1;
    mSelectionRightClicked = false;
    
    if( oldSelection != mSelectionIndex ) {
        fireActionPerformed( this );
        }

    mDelButton.setVisible( false );
    mDelConfirmButton.setVisible( false );

    if( mNumResults == 0 &&
        mSkip > 0 ) {
        
        // we're showing a blank page of results

        // back up until we hit page 0 or find some results to show
        
        mSkip -= PER_PAGE;
        
        if( mSkip < 0 ) {
            mSkip = 0;
            }
        redoSearch( false );
        }
    }


void Picker::focusSearchField() {
    mSearchField.focus();
    }
    
void Picker::clearSearchField() {
    mSearchField.setText( "" );
    }
    
void Picker::setSearchField( const char *inText ) {
    mSearchField.setText( inText );
    redoSearch( true );
    }
    
void Picker::usePickable( int id ) {
    mPickable->usePickable( id );
    }



void Picker::addSearchToStack() {
    char *search = mSearchField.getText();
    
    for( int i=0; i<mPastSearches.size(); i++ ) {
        if( strcmp( search, mPastSearches.getElementDirect( i ) ) == 0 ) {
            
            // found, remove old one
            mPastSearches.deallocateStringElement( i );        
            mPastSearchSkips.deleteElement( i );
            break;
            }
        }
    
    
    mPastSearches.push_back( search );
    mPastSearchSkips.push_back( mSkip );
    
    mPastSearchCurrentIndex = mPastSearches.size() - 1;
    }


        
void Picker::actionPerformed( GUIComponent *inTarget ) {
    int skipAmount = PER_PAGE;
    
    // if( isCommandKeyDown() ) {
        // skipAmount *= 5;
        // }
    if( isShiftKeyDown() ) {
        skipAmount *= 5;
        }
    
    if( inTarget == &mNextButton ) {
        mSkip += skipAmount;
        redoSearch( false );
        addSearchToStack();
        }
    else if( inTarget == &mPrevButton ) {
        mSkip -= skipAmount;
        if( mSkip < 0 ) {
            mSkip = 0;
            }
        redoSearch( false );
        addSearchToStack();
        }
    else if( inTarget == &mSearchField ) {
        mSkip = 0;
        mPastSearchCurrentIndex = mPastSearches.size() - 1;
        redoSearch( false );
        // don't add to stack... not sure that they're done typing yet
        }
    else if( inTarget == &mDelButton ) {
        mDelButton.setVisible( false );
        mDelConfirmButton.setVisible( true );
        }
    else if( inTarget == &mDelConfirmButton ) {    
        mPickable->deleteID( 
            mPickable->getID( mResults[ mSelectionIndex ] ) );
            
        mDelButton.setVisible( false );
        mDelConfirmButton.setVisible( false );

        redoSearch( false );
        }
    
        
    }


void Picker::specialKeyDown( int inKeyCode ) {
    
    if( ! mSearchField.isFocused() ) {
        return;
        }

     switch( inKeyCode ) {
         case MG_KEY_UP:
             if( mPastSearchCurrentIndex > 0 ) {
                 mPastSearchCurrentIndex --;
                 mSearchField.setText( mPastSearches.getElementDirect( 
                                           mPastSearchCurrentIndex ) );
                 mSkip = mPastSearchSkips.getElementDirect( 
                     mPastSearchCurrentIndex );
                 
                 redoSearch( false );
                 }
             break;
         case MG_KEY_DOWN:
             if( mPastSearchCurrentIndex < mPastSearches.size() - 1 ) {
                 mPastSearchCurrentIndex ++;
                 mSearchField.setText( mPastSearches.getElementDirect( 
                                           mPastSearchCurrentIndex ) );
                 mSkip = mPastSearchSkips.getElementDirect( 
                     mPastSearchCurrentIndex );
                 
                 redoSearch( false );
                 }
             break;
         }
    }



void Picker::setIgnoredKey( unsigned char inASCII ) {
    
    mSearchField.setIgnoredKey( inASCII );
    
    }




void Picker::draw() {
    setDrawColor( 0.75, 0.75, 0.75, 1 );
    
    doublePair bgPos = { 0, -85 };
    
    drawRect( bgPos, 80, 160 );
    


    if( mResults != NULL ) {
        doublePair pos = { -50, 40 };
        
        
        for( int i=0; i<mNumResults; i++ ) {
            if( i == mSelectionIndex ) {
                setDrawColor( 1, 1, 1, 1 );
                doublePair selPos = pos;
                selPos.x = 0;
                drawRect( selPos, 80, 32 );
                }

            setDrawColor( 1, 1, 1, 1 );
            mPickable->draw( mResults[i], pos );
            
            doublePair textPos = pos;
            textPos.x += 52;
            
            setDrawColor( mPickable->getTextColor( mResults[i] ) );
            
            const char *text = mPickable->getText( mResults[i] );
            
            int textLen = strlen( text );
            
            int charsLeft = textLen;
            
            SimpleVector<char*> parts;
            
            while( charsLeft > 0 ) {
                char *part =
                    autoSprintf( "%.9s", &( text[ textLen - charsLeft ] ) );
                charsLeft -= strlen( part );
                
                
                parts.push_back( part );
                }
            
            textPos.y += ( parts.size() - 1 ) * 12 / 2;
            
            for( int j=0; j<parts.size(); j++ ) {
                
                char *text = parts.getElementDirect( j );
                char *trimmed = trimWhitespace( text );
                
                smallFont->drawString( trimmed, 
                                       textPos, alignLeft );
                textPos.y -= 12;

                delete [] trimmed;
                delete [] text;
                }
            
            if( mResultsUnclickable[ i ] ) {
                setDrawColor( 0, 0, 0, 0.65 );
                doublePair selPos = pos;
                selPos.x = 0;
                drawRect( selPos, 80, 32 );
                }

            pos.y -= 64;
            }
        }
    

    setDrawColor( 1, 1, 1, 1 );
    
    doublePair pos = { 0, -85 + 216 };
    
    if( mPickable->isSearchable() ) {    
        smallFont->drawString( "Type . for recent", pos, alignCenter );
        }
    
    }



void Picker::pointerDown( float inX, float inY ) {
    if( inX > -80 && inX < 80 &&
        inY < 75 && inY > -245 ) {
        mPressStartedHere = true;
        }
    }
    
    
    
void Picker::select( int index ) {
    if( index < mNumResults && index >= 0 &&
        mResultsUnclickable[ index ] ) {
        return;
        }
    
    if( index >= mNumResults || index < 0 ) {
        return;
        }
    
    mSelectionIndex = index;
    }
    
void Picker::selectUp() {
    int index = mSelectionIndex - 1;
    
    if( index < mNumResults && index >= 0 &&
        mResultsUnclickable[ index ] ) {
        return;
        }
    
    if( index >= mNumResults || index < 0 ) {
        return;
        }
    
    mSelectionIndex = index;
    }
    
void Picker::selectDown() {
    int index = mSelectionIndex + 1;
    
    if( index < mNumResults && index >= 0 &&
        mResultsUnclickable[ index ] ) {
        return;
        }
    
    if( index >= mNumResults || index < 0 ) {
        return;
        }
    
    mSelectionIndex = index;
    }
    
void Picker::nextPage() {
    if( mNextButton.isVisible() ) actionPerformed( &mNextButton );
    }
    
void Picker::prevPage() {
    if( mPrevButton.isVisible() ) actionPerformed( &mPrevButton );
    }

        

void Picker::pointerUp( float inX, float inY ) {
    if( mPressStartedHere &&
        inX > -80 && inX < 80 &&
        inY < 75 && inY > -245 ) {
        
        mPressStartedHere = false;

        inY -= 40;
        
        inY *= -1;
        
        inY += 32;
        
        int oldSelection = mSelectionIndex;
        
        mSelectionRightClicked = isLastMouseButtonRight();
        
        mSelectionIndex = (int)( inY / 64 );
        
        if( mSelectionIndex < mNumResults && mSelectionIndex >= 0 &&
            mResultsUnclickable[ mSelectionIndex ] ) {
            // unclickable, skip it
            
            mSelectionIndex = oldSelection;
            return;
            }
        

        if( mSelectionIndex >= mNumResults ) {
            mSelectionIndex = -1;
            mSelectionRightClicked = false;
            }
        if( mSelectionIndex < 0 ) {
            mSelectionIndex = -1;
            mSelectionRightClicked = false;
            }

        
        // change or re-click on already-selected one
        if( oldSelection != mSelectionIndex 
            || 
            mSelectionIndex != -1 ) {
            
            fireActionPerformed( this );
            addSearchToStack();
            }

        mDelButton.setVisible( false );
        mDelConfirmButton.setVisible( false );

        if( mSelectionIndex != -1 ) {
            mSearchField.unfocusAll();
            
            int pickedID = mPickable->getID( mResults[ mSelectionIndex ] );
            
            mPickable->usePickable( pickedID );
            
            char *text = mSearchField.getText();
            
            if( strcmp( text, "." ) == 0 ) {
                mSkip = 0;
                redoSearch( false );
                mSelectionIndex = 0;
                }
            delete [] text;

            if( mPickable->canDelete( pickedID ) ) {
                mDelButton.setVisible( true );
                }
            }
        
        }
    mPressStartedHere = false;
    }



int Picker::getSelectedObject( char *inWasRightClick ) {
    if( inWasRightClick != NULL ) {
        *inWasRightClick = mSelectionRightClicked;
        }
    
    if( mSelectionIndex == -1 ) {
        return -1;
        }
    
    return mPickable->getID( mResults[mSelectionIndex] );
    }



void Picker::unselectObject() {
    mSelectionIndex = -1;
    mSelectionRightClicked = false;
    
    mDelButton.setVisible( false );
    mDelConfirmButton.setVisible( false );
    }




void Picker::addFilter( char(*inUnclickableFunc)( int inID ) ) {
    FilterFunction f = { inUnclickableFunc };
                         
    mFilterFuncions.push_back( f );
    }


void Picker::removeFilters() {
    mFilterFuncions.deleteAll();
    }

