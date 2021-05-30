#ifndef LUNARMOD_H
#define LUNARMOD_H

#include "LivingLifePage.h"
#include "hetuwmod.h"
#include <vector>
#include <string>

#include "lunarmod_pathFind.h"

using namespace std;

class node {
	public:
		TransRecord* trans = NULL;
		int product = -2;
		node* nextNode = NULL;
		int cost = -1;
};





class LunarMod
{

public:

	static bool showActiveMods;
	
	static bool vogMode;
	
	static string strUpKey;
	static string strLeftKey;
	static string strDownKey;
	static string strRightKey;
	static unsigned char upKey;
	static unsigned char leftKey;
	static unsigned char downKey;
	static unsigned char rightKey;
	static unsigned char uiKey;
	static unsigned char emoteKey;
	static unsigned char slashDieKey;
	static unsigned char killKey;
	static unsigned char followKey;
	static unsigned char searchKey;
	static unsigned char afkKey;
	static unsigned char returnKey;
	static unsigned char ghostKey;
	
	static int upCount;
	static int leftCount;
	static int downCount;
	static int rightCount;
	
	static bool waitingForVogUpdate;
	
	static GridPos posNOTFOUND;
	
	static bool strIsNumeric(const string& s);
	static float distance( int ax, int ay, int bx, int by );
	static float distance( GridPos a, GridPos b );
	static bool posEqual(GridPos pos1, GridPos pos2);
	static bool isAdjacent(GridPos A, GridPos B);
	static bool isOnHorse( LiveObject* player );
	static bool isRegeneratingFood(int id);
	static bool isKilling( LiveObject* player );
	static bool isWeapon( int id );

	static void setLivingLifePage(
		LivingLifePage *inLivingLifePage, 
		SimpleVector<LiveObject> *inGameObjects, 
		int inmMapD,
		int inPathFindingD,
		SimpleVector<int> *inmMapContainedStacks,
		SimpleVector<SimpleVector<int>> *inmMapSubContainedStacks
		);
	static LivingLifePage *livingLifePage;
	static LiveObject *ourLiveObject;
	static SimpleVector<LiveObject> *gameObjects;
	static int maxObjects;
	static int mMapD;
	static int pathFindingD;
	static SimpleVector<int> *mMapContainedStacks;
	static SimpleVector<SimpleVector<int>> *mMapSubContainedStacks;
	static char* bMap;

	static void initOnServerJoin();
	static void initOnBirth();
	static void refreshDirtyMap();
	static void updateDirtyMap();
	static void livingLifeStep();
	static bool livingLifeKeyDown(unsigned char inASCII);
	static bool livingLifeKeyUp(unsigned char inASCII);
	static void livingLifeDraw(float mouseX, float mouseY);
	static bool livingLifePageMouseDown(float inX, float inY);
	
	static int mouseX;
	static int mouseY;
	static int currentX;
	static int currentY;
	static GridPos currentPos;
	
	static void updateBlockedMap(char* &blockedMap, bool considerDoorBlocking = false);
	static bool tileIsSafeToWalk( int x, int y, bool considerDoorBlocking = false );
	static int getClosestPlayerID( int x, int y );
	static GridPos getClosestTile(GridPos src, int objId, bool skipCurrentPos = false);	
	static GridPos getClosestTile(GridPos src, int objId, vector<GridPos> skipPoses);	
	static GridPos getClosestTile(GridPos src, bool* searchArray, vector<GridPos> skipPoses);	
	static GridPos getClosestTileIgnoringUses(GridPos src, int objId);
	typedef struct {
		int searchBoundNegX = 0;
		int searchBoundPosX = 0;
		int searchBoundNegY = 0;
		int searchBoundPosY = 0;
		int nextSearchDirection = 0;
	} spiralSearchStatus;
	static void updateSpiralSearchStatus(spiralSearchStatus* status);
	static GridPos getSpiralSearchPos(spiralSearchStatus* status);
	static GridPos getCloseEnoughTile(GridPos dest);
	static bool hasNaturalAncestorIter(ObjectRecord* o, GridPos pos, vector<int> idHistory);
	static bool hasNaturalAncestor(ObjectRecord* o, GridPos pos);
	static bool isDefinitelyHumanMade(ObjectRecord* o, GridPos pos);
	static bool advSearchArray(ObjectRecord* o, const char* cSearchWord);
	static bool isClosedDoor(ObjectRecord* o);
	static int getDummyParent(int objId);
	static int getDummyUse(int objId);
	
	static int* becomesFoodID;
	static int* initBecomesFood();
	static bool isCategory(int objId);
	static void becomesFood( int checkingID, int sourceID, int depth );
	
	static int dirtyHoldingID;
	static SimpleVector<int> dirtyHoldingStack;
	static int *dirtyMap;
	static bool dirtyUpdateDelay;
	static int lastMapOffsetX;
	static int lastMapOffsetY;
	static int parseOneSecDecay( int objId );
	static bool isSameObject( 
		ObjectRecord* a, ObjectRecord* b,
		SimpleVector<int> contA, SimpleVector<int> contB //, 
		// SimpleVector<SimpleVector<int>> subContA, SimpleVector<SimpleVector<int>> subContB
		);
	static bool objIdReverseAction( int objId );
	static bool objIdReverseActionServer( int objId );
	static void actionAlphaRelativeToMe( int x, int y );
	static void actionBetaRelativeToMe( int x, int y );
	static void useBackpack(bool replace);
	static void useOnSelf();
	static void updateDirtyVars( char modClick, int x, int y, bool vanilla = true );
	static void updateDirtyVarsUseClothing( int clothingIndex, bool replace );
	static void updateDirtyVarsUseOnSelf( int clothingIndex );
	static void refreshDirtyVars();
    static int getObjIdFromDirtyMapI( int inMapI );
	
	
	
	static void drawPoint(doublePair posCen, string color);
	static void drawStr(
		string str,
		doublePair pos,
		string anchorPos = "TL",
		string font = "handwritten",
		float fontScale = 1.0f,
		// TextAlignment align,
		bool withBackground = true,
		bool avoidOffScreen = true
		);
	static void drawTileRect( int x, int y, string color, bool flashing );
	static void drawPath( GridPos* path, int pathLength );
	



	typedef struct {
		
		GridPos* autoMovePath = NULL;
		int autoMovePathLength = 0;
		
		bool waitForDoorToOpen = false;
		GridPos lastDoorPos = posNOTFOUND;
		bool waitingToDrop = false;
		GridPos lastDropPos = posNOTFOUND;
		bool waitingToGet = false;
		bool waitingToUseOnSelf = false;
		
		bool wasOnHorse = false;
		int rideObjId = 0;
		GridPos horsePos = posNOTFOUND;		
		
		bool droppingHorse = false;
		bool droppingHorse_allowWalkBack = true;
		GridPos targetPos = posNOTFOUND;
		int droppingHorse_result = 1;
		
		bool gettingBackOnHorse = false;
		
		
		string automoveDirection = "";
		
		GridPos autoSearchReachPos = posNOTFOUND;
		
		vector<GridPos> foodFoundPosHistory;
		spiralSearchStatus* autoEatSearchStatus;
		
		GridPos lastFoodPos = posNOTFOUND;
		int eatingPhaseNumber = 0;
		bool eatingPhaseOn = false;
		
		
		vector<TransRecord*> autoCraftTree;
		int autoCraftIndex = -1;
		GridPos autoCraftPos = posNOTFOUND;
		bool autoCraftDropMade = false;
		int autoCraftDropMadeResult = 1;
		
	} moveStatus;

	

	static void sendAction(string action, int x, int y, int extra = -1, int extra2 = -1, bool fast = true);
	static void sendMove(GridPos* path, int pathLength);
	static void sendStep(int startX, int startY, int relativeX, int relativeY);
	
	static int move(int destX, int destY);
	static void moveReset();
	static int eat(GridPos pos);
	static int use(GridPos pos);
	static int pick(GridPos pos);
	static int drop(GridPos pos);
	static int yeet(GridPos pos);
	static int moveAndPick(GridPos dest, bool force = true);
	static int moveAndYeet(GridPos dest, bool force = true);
	
	static void openDoor(int doorX, int doorY);
	static int dropHorse();
	static int getHorse();
	static int useBackpack();
	
	
	
	static void allModsOff();
	
	static bool emoteSpamMode;
	static int emoteSpamIndex;
	static int currentEmote;
	
	static double emoteSpamLastTime;
	
	
	static void autoMove();
	static bool autoMode;
	static moveStatus* currentMoveStatus;
	
	static void autoReturn();
	static bool returnMode;
	static bool nextCharForDest;
	static float savedX;
	static float savedY;
	
	static void autoFollow(int playerID, float dist = 0.0f);
	static bool followMode;
	static int followDistanceIndex;
	static int mouseOverPlayerID;
	static int playerID;
	
	static int autoSearch(bool* searchArray, vector<GridPos> &historyArray, bool active = false, bool verbose = true);
	static void autoSearchHistoryReset();
	static void autoSearchContinue();
	static bool searchMode;
	static spiralSearchStatus* autoSearchStatus;
	static vector<GridPos> foundPosHistory;
	static vector<char*> presets;

	static bool autoEat(bool wasteFood, bool active);
	static bool afkMode;
	static bool afkModeVerbose;
	static bool* foodSearchArray;

	static bool ghostMode;
	static bool wakingMode;
	static int ghostMove(int destX, int destY);
	static bool ghostModeAwake();

	static bool autoKill(int playerID);
	static int killModeIndex;
	static bool inKillingMode_me;
	static bool inKillingMode_victim;
	static bool victimKillFirst;

	
	static char* d;
	static bool testMode;
	static int testCount;
	static GridPos* testPath;
	static GridPos testTile;
	static vector<GridPos> testTiles;
	
	
	
	
	static bool killMode;
	
	
	



	
	static void loading();
	
	static string getObjectName(int id);
	static bool isProbabilitySet(int objId);
	static bool transEqual(TransRecord* a, TransRecord* b);
	static void printTrans(TransRecord* t, bool printName = true, bool eof = true);
	static int getUseDummyParent(int objId);
	static bool transEqualIgnoringUse(TransRecord* a, TransRecord* b);
	
	static vector<TransRecord*> allTrans;
	static vector<TransRecord*> getUsesTrans( int objId );
	static vector<TransRecord*> getProdTrans( int objId );
	static int getTransIndex( vector<TransRecord*> transVector, TransRecord* trans );
	
	static vector<int> sizeMap;
	static vector<int> depthMap;
	static vector<TransRecord*> techMap;
	static void initTechMap();
	static vector<int> undoMap;
	static void initUndoMap();
	static int costFromTransMap( vector<TransRecord*> trans, int objId );
	static int computeUndoCostMap( TransRecord *trans );
	
	static vector<TransRecord*> computeTransMap( int objId, vector<bool> givenObjs );
	static vector<TransRecord*> treeFromTransMap( vector<TransRecord*> trans, int objId );
	
	static void printTree( vector<TransRecord*> transMap );
	
	
	
	static TransRecord* resolveTechTree( vector<TransRecord*> trans, int objId);	
	

	static int moveObj(GridPos fromPos, GridPos toPos, int objId);

};


#endif
