// #include <windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <random> //emoteSpam
#include <regex> //advSearch

#include "groundSprites.h"
#include "LivingLifePage.h"
#include "objectBank.h"
#include "categoryBank.h"
#include "emotion.h"
#include "ageControl.h"

#include "minorGems/system/Time.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/game/drawUtils.h"
#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/util/SettingsManager.h" //presets

#include "hetuwmod.h"
#include "lunarmod.h"

#include "phex.h"

#include "minitech.h"

bool LunarMod::showActiveMods = true;
bool LunarMod::vogMode = false;
string LunarMod::strUpKey;
string LunarMod::strLeftKey;
string LunarMod::strDownKey;
string LunarMod::strRightKey;
unsigned char LunarMod::upKey = 'w';
unsigned char LunarMod::leftKey = 'a';
unsigned char LunarMod::downKey = 's';
unsigned char LunarMod::rightKey = 'd';
unsigned char LunarMod::uiKey = 'o';
unsigned char LunarMod::emoteKey = 'i';
unsigned char LunarMod::slashDieKey = 'm';
unsigned char LunarMod::killKey = 't';
unsigned char LunarMod::followKey = 'g';
unsigned char LunarMod::searchKey = 'r';
unsigned char LunarMod::afkKey = 'v';
unsigned char LunarMod::returnKey = 'z';
unsigned char LunarMod::ghostKey = 'n';

int LunarMod::upCount;
int LunarMod::leftCount;
int LunarMod::downCount;
int LunarMod::rightCount;

bool LunarMod::waitingForVogUpdate = false;

GridPos LunarMod::posNOTFOUND = {9999, 9999};

LivingLifePage *LunarMod::livingLifePage;
LiveObject *LunarMod::ourLiveObject;
SimpleVector<LiveObject> *LunarMod::gameObjects;
int LunarMod::maxObjects;
int LunarMod::mMapD;
int LunarMod::pathFindingD;
SimpleVector<int> *LunarMod::mMapContainedStacks;
SimpleVector<SimpleVector<int>> *LunarMod::mMapSubContainedStacks;
char* LunarMod::bMap = NULL;

int LunarMod::mouseX;
int LunarMod::mouseY;
int LunarMod::currentX;
int LunarMod::currentY;
GridPos LunarMod::currentPos;

bool LunarMod::emoteSpamMode;
int LunarMod::emoteSpamIndex;
int LunarMod::currentEmote;
double LunarMod::emoteSpamLastTime;
std::random_device rd;
std::mt19937 rng(rd());

bool LunarMod::autoMode;
LunarMod::moveStatus* LunarMod::currentMoveStatus;

bool LunarMod::returnMode;
bool LunarMod::nextCharForDest;
float LunarMod::savedX;
float LunarMod::savedY;

bool LunarMod::followMode;
int LunarMod::followDistanceIndex;
int LunarMod::mouseOverPlayerID;
int LunarMod::playerID = 0; //BUG

bool LunarMod::searchMode;
LunarMod::spiralSearchStatus* LunarMod::autoSearchStatus;
vector<GridPos> LunarMod::foundPosHistory;
vector<char*> LunarMod::presets;
	
bool LunarMod::afkMode;
bool LunarMod::afkModeVerbose;
bool* LunarMod::foodSearchArray;

bool LunarMod::ghostMode;
bool LunarMod::wakingMode;

bool LunarMod::killMode;
int LunarMod::killModeIndex;
bool LunarMod::inKillingMode_me;
bool LunarMod::inKillingMode_victim;
bool LunarMod::victimKillFirst;

int LunarMod::dirtyHoldingID;
SimpleVector<int> LunarMod::dirtyHoldingStack;
int *LunarMod::dirtyMap = NULL;
bool LunarMod::dirtyUpdateDelay;
int LunarMod::lastMapOffsetX;
int LunarMod::lastMapOffsetY;


char* LunarMod::d;
bool LunarMod::testMode;
int LunarMod::testCount;
GridPos* LunarMod::testPath;
GridPos LunarMod::testTile;
vector<GridPos> LunarMod::testTiles;









vector<TransRecord*> LunarMod::allTrans;
vector<int> LunarMod::sizeMap;
vector<int> LunarMod::depthMap;
vector<TransRecord*> LunarMod::techMap;
vector<int> LunarMod::undoMap;







void LunarMod::setLivingLifePage(
	LivingLifePage *inLivingLifePage, 
	SimpleVector<LiveObject> *inGameObjects, 
	int inmMapD, 
	int inPathFindingD,
	SimpleVector<int> *inmMapContainedStacks,
	SimpleVector<SimpleVector<int>> *inmMapSubContainedStacks
	) {
	
	livingLifePage = inLivingLifePage;
	gameObjects = inGameObjects;
	maxObjects = getMaxObjectID() + 1;
	mMapD = inmMapD;
	pathFindingD = inPathFindingD;
	mMapContainedStacks = inmMapContainedStacks;
	mMapSubContainedStacks = inmMapSubContainedStacks;
}

void LunarMod::initOnServerJoin() {
	initOnBirth();
}

void LunarMod::initOnBirth() {
	
	strUpKey = "";
	strLeftKey = "";
	strDownKey = "";
	strRightKey = "";
	strUpKey.push_back(toupper(upKey));
	strLeftKey.push_back(toupper(leftKey));
	strDownKey.push_back(toupper(downKey));
	strRightKey.push_back(toupper(rightKey));
	
	upCount = -1;
	leftCount = -1;
	downCount = -1;
	rightCount = -1;
	
	
	if (bMap != NULL) {
		delete [] bMap;
		// bMap = NULL;
	}
	int numPathMapCells = pathFindingD * pathFindingD;
	bMap = new char[ numPathMapCells ];
	
	
	emoteSpamMode = false;
	emoteSpamIndex = 0;
	currentEmote = 0;
	
	moveReset();
	
	autoMode = false;
	
	returnMode = false;
	nextCharForDest = false;
	savedX = 0;
	savedY = 0;
	
	followMode = false;
	followDistanceIndex = 0;
	mouseOverPlayerID = 0;
	// not resetting playerID
	
	
	searchMode = false;
	
	
	ourLiveObject = livingLifePage->getOurLiveObject();
	if ( ourLiveObject ) {
		refreshDirtyMap();
	}
	
	
	
	if (foodSearchArray != NULL) delete[] foodSearchArray;
	foodSearchArray = new bool[maxObjects];
	for (int i=0; i<maxObjects; i++) {
		foodSearchArray[i] = isRegeneratingFood(i);
		
		ObjectRecord* o = getObject(i);
		if (o != NULL) {
			foodSearchArray[i] = 
				foodSearchArray[i] || 
				advSearchArray( o, "##FOOD");
		}
	}
	

	
	
	

	// delete d;
	d = new char[1024];
	memset(d, 0, 32);
	

	char *dirChars = SettingsManager::getDirectoryName();
    std::string dir(dirChars);
    delete [] dirChars;
	
	for (int i=0; i<10; i++) {
		std::string presetFilename = "advSearchPreset" + to_string(i);
		std::string presetPath =  dir + "/" + presetFilename + ".ini";
		std::string presetName = "";
		
		if (FILE *userEmailFile = fopen(presetPath.c_str(), "r")) {
			char *cont = SettingsManager::getSettingContents( presetFilename.c_str(), "" );
			presets.push_back(cont);
            delete [] cont;
		} else {
			break;
		}
	}
	
	
	
}

void LunarMod::refreshDirtyMap() {
    if ( dirtyMap != NULL ) {
        delete [] dirtyMap;
        // dirtyMap = NULL;
    }
    dirtyMap = new int[ mMapD * mMapD ];
    
    for( int i=0; i<mMapD *mMapD; i++ ) {
        dirtyMap[i] = -1;
    }
    
    memcpy( dirtyMap, livingLifePage->mMap, mMapD * mMapD * sizeof( int ) );
}

void LunarMod::updateDirtyMap() {
    dirtyHoldingID = ourLiveObject->holdingID;
    refreshDirtyMap();
    dirtyUpdateDelay = false;
}


bool LunarMod::strIsNumeric(const string& s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

float LunarMod::distance( int ax, int ay, int bx, int by ) {
	return sqrt(pow(ay - by, 2) + pow(ax - bx, 2));
}

float LunarMod::distance( GridPos a, GridPos b ) {
	return sqrt(pow(a.y - b.y, 2) + pow(a.x - b.x, 2));
}

bool LunarMod::posEqual(GridPos pos1, GridPos pos2) {
	return 
		pos1.x == pos2.x &&
		pos1.y == pos2.y;
}

bool LunarMod::isAdjacent( GridPos A, GridPos B ) {
    return
		( abs( A.x - B.x ) == 1 && A.y == B.y ) ||
        ( abs( A.y - B.y ) == 1 && A.x == B.x );
}

bool LunarMod::isOnHorse( LiveObject* player ) {
	int holdingID = player->holdingID;
	ObjectRecord* obj = getObject(holdingID);
	if (obj == NULL) return false;
	return obj->rideable && obj->speedMult > 0.0;
}

bool LunarMod::isRegeneratingFood(int id) {
	TransRecord* pick_trans = getTrans( 0, id );
	if (pick_trans == NULL) return false;
	int food_id = pick_trans->newActor;
	if (food_id == 837) return false; //We dont eat mushroom...
	ObjectRecord* food = getObject(food_id);
	if (food == NULL) return false;
	if (food->foodValue <= 0) return false;
	
	int newTarget = pick_trans->newTarget;
	TransRecord* regen_trans = getTrans( -1, newTarget );
	if ( regen_trans == NULL ) return false;
	bool timeChangeExist = regen_trans->autoDecaySeconds != 0;
	int i = 0;
	while ( timeChangeExist && i < 10 ) {
		newTarget = regen_trans->newTarget;
		if ( newTarget == id ) return true;
		regen_trans = getTrans( -1, newTarget );
		if ( regen_trans == NULL ) return false;
		timeChangeExist = regen_trans->autoDecaySeconds != 0;
		i++;
	}
	return false;
}

bool LunarMod::isKilling( LiveObject* player ) {
	bool killing;
	if ( player->currentEmot == NULL ) {
		killing = false;
	} else {
		char keyword[] = "MURDERFACE";
		killing = strstr( player->currentEmot->triggerWord, keyword ) != NULL;
	}
	return killing;
}

bool LunarMod::isWeapon( int id ) {
	ObjectRecord *o = getObject( id );
	if ( o != NULL ) {
		if ( o->deadlyDistance > 0 ) {
			return true;
		}
	}
	return false;
}


void LunarMod::updateBlockedMap(char* &blockedMap, bool considerDoorBlocking) {
	
	int mMapOffsetX = livingLifePage->mMapOffsetX;
	int mMapOffsetY = livingLifePage->mMapOffsetY;
	int *mMap = livingLifePage->mMap;
    int pathOffsetX = pathFindingD/2 - currentX;
    int pathOffsetY = pathFindingD/2 - currentY;	
	
	for( int y=0; y<pathFindingD; y++ ) {
		int mapY = ( y - pathOffsetY ) + mMapD / 2 - mMapOffsetY;
		int posY = ( y - pathOffsetY );
		
		for( int x=0; x<pathFindingD; x++ ) {
			int mapX = ( x - pathOffsetX ) + mMapD / 2 - mMapOffsetX;
			int posX = ( x - pathOffsetX );
			
			if( mapY >= 0 && mapY < mMapD &&
				mapX >= 0 && mapX < mMapD ) { 

				int mapI = mapY * mMapD + mapX;
				
				if( mMap[ mapI ] == -1) {
					blockedMap[ y * pathFindingD + x ] = true;
					continue;
				}
				
				if( !tileIsSafeToWalk(posX, posY, considerDoorBlocking) ) {
					blockedMap[ y * pathFindingD + x ] = true;
				}
				
				if( mMap[ mapI ] > 0 ) {
					ObjectRecord *o = getObject( mMap[ mapI ] );
					
					if( o->wide ) {
						
						for( int dx = - o->leftBlockingRadius;
							 dx <= o->rightBlockingRadius; dx++ ) {
							
							int newX = x + dx;
							
							if( newX >=0 && newX < pathFindingD ) {
								blockedMap[ y * pathFindingD + newX ] = true;
							}
						}
					}
				}
			}
		}
	}
}

bool LunarMod::tileIsSafeToWalk(int x, int y, bool considerDoorBlocking) {
	if (!HetuwMod::tileHasNoDangerousAnimals(x, y)) return false;
	int objId = livingLifePage->hetuwGetObjId(x, y);
	ObjectRecord* obj = getObject(objId);
	if (obj && obj->blocksWalking) {
		if (!considerDoorBlocking && isClosedDoor(obj)) return true;
		return false;
	}
	return true;
}

int LunarMod::getClosestPlayerID( int x = 1977, int y = 1977 ) {
	if ( x == 1977 || y == 1977 ) {
		x = currentX;
		y = currentY;
	}
	float bestDist = 9999.0;
	int closestPlayerID = 0;
	for (int i=0; i<gameObjects->size(); i++) {
		LiveObject *player = gameObjects->getElement(i);
		if ( player == NULL ) continue;
		if ( player == ourLiveObject ) continue;
		if ( player->currentPos.x == 1977.0 && player->currentPos.y == 1977.0) continue;
		float dist = distance(x, y, player->currentPos.x, player->currentPos.y);

		if (dist < bestDist) {
			bestDist = dist;
			closestPlayerID = player->id;
		}
	}
	return closestPlayerID;
}

GridPos LunarMod::getClosestTile(GridPos src, int objId, bool skipCurrentPos) {
	bool* searchArray = new bool[maxObjects];
	for (int i=0; i<maxObjects; i++) {
		searchArray[i] = i == objId;
	}
	vector<GridPos> skipPoses;
	if (skipCurrentPos) skipPoses.push_back(currentPos);	
	GridPos result = getClosestTile(src, searchArray, skipPoses);
	delete [] searchArray;
	return result;
}

GridPos LunarMod::getClosestTile(GridPos src, int objId, vector<GridPos> skipPoses) {
	bool* searchArray = new bool[maxObjects];
	for (int i=0; i<maxObjects; i++) {
		searchArray[i] = i == objId;
	}
	GridPos result = getClosestTile(src, searchArray, skipPoses);
	delete [] searchArray;
	return result;
}

GridPos LunarMod::getClosestTile(GridPos src, bool* searchArray, vector<GridPos> skipPoses) {
	
	int *mMap = livingLifePage->mMap;
	int mMapOffsetX = livingLifePage->mMapOffsetX;
	int mMapOffsetY = livingLifePage->mMapOffsetY;
	int pathOffsetX = pathFindingD/2 - currentX;
	int pathOffsetY = pathFindingD/2 - currentY;
	
	float bestDist = 9999.0;
	GridPos foundPos = posNOTFOUND;
	for( int y=0; y<pathFindingD; y++ ) {
		int mapY = ( y - pathOffsetY ) + mMapD / 2 - mMapOffsetY;
		
		for( int x=0; x<pathFindingD; x++ ) {
			int mapX = ( x - pathOffsetX ) + mMapD / 2 - mMapOffsetX;
			
			if( mapY >= 0 && mapY < mMapD &&
				mapX >= 0 && mapX < mMapD ) { 
				
				int posX = mapX + mMapOffsetX - mMapD / 2;
				int posY = mapY + mMapOffsetY - mMapD / 2;
				GridPos pos = {posX, posY};
				
				float dist = distance(src, pos);
				if (dist >= bestDist) continue;
				
				bool skip = false;
				for (unsigned int i=0; i<skipPoses.size(); i++) {
					if ( posEqual( pos, skipPoses[i] ) ) skip = true;
				}
				if (skip) continue;

				int mapI = mapY * mMapD + mapX;
				int id = mMap[mapI];
				if (id < 0 || id >= maxObjects) continue;
				bool found = false;

				if (searchArray[id]) {
					found = true;
					foundPos = pos;
					bestDist = dist;
					continue;
				}
				
				if (mMapContainedStacks[mapI].size() > 0) {
					for (int i=0; i < mMapContainedStacks[mapI].size(); i++) {
						id = mMapContainedStacks[mapI].getElementDirect(i);
						if (searchArray[id]) {
							found = true;
							foundPos = pos;
							bestDist = dist;
						}
					}
					if (found) continue;
				}
				
				if (mMapSubContainedStacks[mapI].size() > 0) {
					for (int i=0; i < mMapSubContainedStacks[mapI].size(); i++) {
						SimpleVector<int> subContainedStack = mMapSubContainedStacks[mapI].getElementDirect(i);
						for (int k=0; k < subContainedStack.size(); k++) {
							id = subContainedStack.getElementDirect(k);
							if (searchArray[id]) {
								found = true;
								foundPos = pos;
								bestDist = dist;
							}
						}
					}
					if (found) continue;
				}
				
			}
		}
	}
	
	return foundPos;
}

GridPos LunarMod::getClosestTileIgnoringUses(GridPos src, int objId) {
	if (objId <= 0) return getClosestTile(src, 0);
	vector<int> ids;
	// ObjectRecord* obj = getObject(objId);
	// if ( obj->numUses > 0 ) {
		// ids.push_back(objId);
		// for (int i=0; i<obj->numUses-1; i++) {
			// ids.push_back(obj->useDummyIDs[i]);
		// }
	// } else if ( isUseDummy(objId) ) {
		// int parentObjId = obj->useDummyParent;
		// ObjectRecord* parentObj = getObject(parentObjId);
		// ids.push_back(parentObjId);
		// for (int i=0; i<parentObj->numUses-1; i++) {
			// ids.push_back(parentObj->useDummyIDs[i]);
		// }
	// } else {
		ids.push_back(objId);
	// }
	
	bool* searchArray = new bool[maxObjects];
	for (int i=0; i<maxObjects; i++) {
		searchArray[i] = false;
		for (int j=0; j<ids.size(); j++) {
			if ( i == ids[j] ) searchArray[i] = true;
		}
	}
	vector<GridPos> skipPoses;
	GridPos result = getClosestTile(src, searchArray, skipPoses);
	delete [] searchArray;
	return result;
}

void LunarMod::updateSpiralSearchStatus(spiralSearchStatus* status) {
	int *mMap = livingLifePage->mMap;
	int mMapOffsetX = livingLifePage->mMapOffsetX;
	int mMapOffsetY = livingLifePage->mMapOffsetY;
	int pathOffsetX = pathFindingD/2 - currentX;
	int pathOffsetY = pathFindingD/2 - currentY;

	for( int y=0; y<pathFindingD; y++ ) {
		int mapY = ( y - pathOffsetY ) + mMapD / 2 - mMapOffsetY;
		int posY = mapY + mMapOffsetY - mMapD / 2;
		
		for( int x=0; x<pathFindingD; x++ ) {
			int mapX = ( x - pathOffsetX ) + mMapD / 2 - mMapOffsetX;
			int posX = mapX + mMapOffsetX - mMapD / 2;
			
			if( mapY >= 0 && mapY < mMapD &&
				mapX >= 0 && mapX < mMapD ) { 

				int mapI = mapY * mMapD + mapX;
			
			
				if (mMap[ mapI ] != -1) {
					
					if (posX - pathFindingD/8 < status->searchBoundNegX) status->searchBoundNegX = posX - pathFindingD/8;
					if (posX + pathFindingD/8 > status->searchBoundPosX) status->searchBoundPosX = posX + pathFindingD/8;
					if (posY - pathFindingD/8 < status->searchBoundNegY) status->searchBoundNegY = posY - pathFindingD/8;
					if (posY + pathFindingD/8 > status->searchBoundPosY) status->searchBoundPosY = posY + pathFindingD/8;
				
				}
			}										
		}
	}

	status->nextSearchDirection = (status->nextSearchDirection + 1) % 4;
}

GridPos LunarMod::getSpiralSearchPos(spiralSearchStatus* status) {
	if (status->nextSearchDirection == 0) return {status->searchBoundPosX, status->searchBoundPosY};
	if (status->nextSearchDirection == 1) return {status->searchBoundNegX, status->searchBoundPosY};
	if (status->nextSearchDirection == 2) return {status->searchBoundNegX, status->searchBoundNegY};
	if (status->nextSearchDirection == 3) return {status->searchBoundPosX, status->searchBoundNegY};
	return posNOTFOUND;
}

GridPos LunarMod::getCloseEnoughTile(GridPos dest) {
	int pathLength = 0;
	GridPos* path = NULL;
	GridPos closestFound = {currentX, currentY};
	bool pathFound = false;
	
	pathFound = LunarModPathFind( pathFindingD, pathFindingD,
								bMap, 
								currentPos, dest,
								&( pathLength ), 
								&( path ),
								&( closestFound )
								);
								
	GridPos targetPos = closestFound;
	
	if ( !pathFound ) {
		if ( !isAdjacent(dest, closestFound) ) {
			return posNOTFOUND;
		}
		targetPos = closestFound;
	} else {
		if ( pathLength >= 2 ) {
			if ( isAdjacent(dest, path[pathLength - 2]) ) targetPos = path[pathLength - 2];
		}
	}
	
	return targetPos;
}

bool LunarMod::hasNaturalAncestorIter(ObjectRecord* o, GridPos pos, vector<int> idHistory) {
	
	if ( idHistory.size() > 0 ) {
		for ( unsigned int i=0; i<idHistory.size(); i++ ) {
			if ( idHistory[i] == o->id ) return false;
		}
	}
	
	SimpleVector<TransRecord*> *prodTrans = getAllProduces( o->id );
	int numTrans = 0;
	if( prodTrans != NULL ) {
		numTrans = prodTrans->size();
	}
	if( numTrans == 0 ) {
		return false; 
	}
	bool couldHaveNaturalAncestor = false;
	for( int t=0; t<numTrans; t++ ) {
		TransRecord *trans = prodTrans->getElementDirect( t );
		if( trans->actor == -1 ) {
			int tarId = trans->target;
			ObjectRecord* tar = getObject(tarId);
			if ( tar->mapChance > 0 ) {
				int mapI = livingLifePage->hetuwGetMapI( pos.x, pos.y );
				int posBiome = livingLifePage->mMapBiomes[mapI];
				bool couldBeNaturallySpawned = false;
				for ( int i=0; i<tar->numBiomes; i++ ) {
					couldBeNaturallySpawned = couldBeNaturallySpawned || (posBiome == tar->biomes[i]);
				}
				if (couldBeNaturallySpawned) return true;
			}
			idHistory.push_back(o->id);
			// printf("DEBUG RESULT: d:%d, o:%d, t:%d\n", depth, o->id, tarId);
			couldHaveNaturalAncestor = couldHaveNaturalAncestor || hasNaturalAncestorIter(tar, pos, idHistory);
		}
	}
	return couldHaveNaturalAncestor;
}

bool LunarMod::hasNaturalAncestor(ObjectRecord* o, GridPos pos) {
	vector<int> idHistory;	
	return hasNaturalAncestorIter(o, pos, idHistory);
}

bool LunarMod::isDefinitelyHumanMade(ObjectRecord* o, GridPos pos) {
	int mapI = livingLifePage->hetuwGetMapI( pos.x, pos.y );
	if (mapI < 0) return false;
	int posBiome = livingLifePage->mMapBiomes[mapI];
	if (posBiome < 0) return false;
	bool couldBeNaturallySpawned = false;
	if ( o->mapChance > 0 ) {
		for ( int i=0; i<o->numBiomes; i++ ) {
			couldBeNaturallySpawned = couldBeNaturallySpawned || (posBiome == o->biomes[i]);
		}
	}
	if (couldBeNaturallySpawned) return false;
	return !hasNaturalAncestor(o, pos);
}

bool LunarMod::advSearchArray(ObjectRecord* o, const char* cSearchWord) {
	string rawSearchWord(cSearchWord);
	string description(o->description);
	for (auto & c: rawSearchWord) c = toupper(c);
	for (auto & c: description) c = toupper(c);
	
	if ( cSearchWord[0] == '#' && rawSearchWord.length() > 1 ) {
		
		if ( cSearchWord[1] == '#' ) {
			//preset
			string searchWord(rawSearchWord.begin()+2, rawSearchWord.end());
			
			if ( searchWord == "CLOTHES" ) return o->clothing != 'n' && getObjectDepth(o->id) != 0;
			if ( searchWord == "CART" ) {
				if ( !o->permanent ) {
					return o->rideable && o->speedMult > 0.0 && !o->floor;
				} else {
					TransRecord* t = getTrans( 0, o->id );
					if ( t != NULL ) {
						ObjectRecord* c = getObject(t->newActor);
						if ( c != NULL ) {
							return c->rideable && c->speedMult > 0.0 && !c->floor;
						}
					}
					return false;
				}
			}
			
			char* cont;
			int numParts;
			char** parts;
			bool presetFound = false;
			
			for ( unsigned int i=0; i<presets.size(); i++) {
				cont = presets[i];
				parts = split( cont, "\n", &numParts );
				if ( numParts > 1 ) {
					string presetName = parts[0];
					for (auto & c: presetName) c = toupper(c);
					if ( searchWord == presetName ) {
						presetFound = true;
						break;
					}
				}
			}
			if (!presetFound) return false;
			
			bool beingSearched = false;
			for( int i=1; i<numParts; i++ ) {
				if( strcmp( parts[i], "" ) != 0 ) {
					beingSearched = beingSearched || advSearchArray(o, parts[i]);
				}
                delete [] parts[i];
			}
            delete [] parts;

			return beingSearched;
			
		} else {
			string searchWord(rawSearchWord.begin()+1, rawSearchWord.end());
			if ( strIsNumeric(searchWord) ) {
				//numeric
				return stoi(searchWord) == o->id;
			} else {
				//exact
				std::regex r("\\b" + searchWord + "\\b"); // the pattern \b matches a word boundary
				std::smatch m;
				
				if ( std::regex_search(description, m, r) ) {
					return true;
				} else {
					return false;
				}
			}
		}
	}
	
	return HetuwMod::charArrContainsCharArr(description.c_str(), cSearchWord);
	
}
	
bool LunarMod::isClosedDoor( ObjectRecord* o ) {
	if ( o == NULL || o->description == NULL ) return false;
	if ( !o->blocksWalking ) return false;
	if( strstr( o->description, "+password-protected" ) != NULL ) return false;
	TransRecord* t = getPTrans( 0, o->id );
	if ( t == NULL ) return false;
	ObjectRecord* newTarget = getObject( t->newTarget );
	if ( newTarget == NULL ) return false;
	if ( !newTarget->blocksWalking ) return true;
	return false;
}

int LunarMod::getDummyParent(int objId) {
	if (objId <= 0 || objId >= maxObjects) return objId;
	ObjectRecord* o = getObject(objId);
	if (o != NULL) {
		if (o->isUseDummy) return o->useDummyParent;
	}
	return objId;
}

int LunarMod::getDummyUse(int objId) {
	//return -1 if not applicable
	//return 0 if it is the parent object
	//then 1 is the last use, and 2 is #use 2 etc.
	if (objId <= 0) return -1;
	ObjectRecord* o = getObject(objId);
	if (o == NULL) return -1;
	if (o->numUses > 1) return 0;
	if (o->isUseDummy) {
		return o->thisUseDummyIndex + 1;
	}
	return -1;
}


int* LunarMod::becomesFoodID;

int* LunarMod::initBecomesFood() {
	becomesFoodID = new int[maxObjects];
	for (int id=0; id<maxObjects; id++) {
		becomesFoodID[id] = 0;
	}
	for (int id=0; id<maxObjects; id++) {
		ObjectRecord* o = getObject( id );
		if ( o == NULL ) continue;
		if ( o->foodValue > 0 ) {
			
			// Food themselves
			if( o->isUseDummy ) {
				becomesFoodID[id] = o->useDummyParent;
			} else {
				becomesFoodID[id] = id;
			}
			
			int foodDepth = getObjectDepth( id );
			if (foodDepth == UNREACHABLE) continue;

			// Objects you can pick the food from
			SimpleVector<TransRecord*> *trans = getAllProduces( id );
			if ( trans != NULL && trans->size() > 0 ) {
				for( int i = 0; i<trans->size(); i++ ) {
					TransRecord* t = trans->getElementDirect( i );
					if ( t->actor == 0 ) {
						if ( t->target <= 0 ) continue;
						ObjectRecord* target = getObject( t->target );
						if ( target == NULL ) continue;
						
						if ( becomesFoodID[t->target] == 0 ) becomesFoodID[t->target] = id;
					}
				}
			}
			
			// 3 depth of ancestors that dont have other uses than making this food
			becomesFood( id, id, 3 );
			
		}
	}
	return becomesFoodID;
}

bool LunarMod::isCategory(int objId) {
	if (objId <= 0) return false;
	CategoryRecord *c = getCategory( objId );
	if (c == NULL) return false;
    if( !c->isPattern && c->objectIDSet.size() > 0 ) return true;
    return false;
}

void LunarMod::becomesFood( int checkingID, int sourceID, int depth ) {
	
	if ( checkingID < 0 ) return;
    ObjectRecord* o = getObject( checkingID );
    if ( o == NULL ) return;
	if ( depth < 1 ) return;
	
	SimpleVector<TransRecord*> *prodTrans = getAllProduces( checkingID );
    if ( prodTrans == NULL ) return;
    if ( prodTrans->size() < 1 ) return;
	
	for( int i = 0; i<prodTrans->size(); i++ ) {
		
		TransRecord* t = prodTrans->getElementDirect( i );
		
		if ( t->actor > 0 && t->actor < maxObjects ) {
			SimpleVector<TransRecord*> *useTrans = getAllUses( t->actor );
			if ( useTrans != NULL ) {
				bool irrelevantTrans = false;
				for( int j = 0; j<useTrans->size(); j++ ) {
					TransRecord* s = useTrans->getElementDirect( j );
					// if ( ! livingLifePage->getTransHintable( s ) ) continue;
					bool newActorRelevant = false;
					if ( s->newActor > 0 && s->newActor < maxObjects && becomesFoodID[s->newActor] == sourceID ) newActorRelevant = true;
					bool newTargetRelevant = false;
					if ( s->newTarget > 0 && s->newTarget < maxObjects && becomesFoodID[s->newTarget] == sourceID ) newTargetRelevant = true;
					if ( !newActorRelevant && !newTargetRelevant ) irrelevantTrans = true;
				}
				
				if ( !irrelevantTrans ) {
					if ( becomesFoodID[t->actor] == 0 ) becomesFoodID[t->actor] = sourceID;
					becomesFood( t->actor, sourceID, depth - 1 );
				}
			}
		}
		if ( t->target > 0 && t->target < maxObjects ) {
			SimpleVector<TransRecord*> *useTrans = getAllUses( t->target );
			if ( useTrans != NULL ) {
				bool irrelevantTrans = false;
				for( int j = 0; j<useTrans->size(); j++ ) {
					TransRecord* s = useTrans->getElementDirect( j );
					// if ( ! livingLifePage->getTransHintable( s ) ) continue;
					bool newActorRelevant = false;
					if ( s->newActor > 0 && s->newActor < maxObjects && becomesFoodID[s->newActor] == sourceID ) newActorRelevant = true;
					bool newTargetRelevant = false;
					if ( s->newTarget > 0 && s->newTarget < maxObjects && becomesFoodID[s->newTarget] == sourceID ) newTargetRelevant = true;
					if ( !newActorRelevant && !newTargetRelevant ) irrelevantTrans = true;
				}
				
				if ( !irrelevantTrans ) {
					if ( becomesFoodID[t->target] == 0 ) becomesFoodID[t->target] = sourceID;
					becomesFood( t->target, sourceID, depth - 1 );
				}
			}
		}
		
	}
	return;
}


int LunarMod::parseOneSecDecay( int objId ) {
	if ( objId <= 0 ) return objId;
	TransRecord *r = getTrans( -1, objId );
	if ( r == NULL ) return objId;
	if ( r->autoDecaySeconds != 1 ) return objId;
	return r->newTarget;
}

bool LunarMod::isSameObject( 
	ObjectRecord* a, ObjectRecord* b,
	SimpleVector<int> contA, SimpleVector<int> contB //, 
	// SimpleVector<SimpleVector<int>> subContA, SimpleVector<SimpleVector<int>> subContB
	) {
	
	if ( a->id != b->id ) return false;
	if ( a->written || b->written ) return false;
	if ( a->numSlots == 0 ) return true;
	if ( contA.size() != contB.size() ) return false;
	
	for ( int i = 0; i < contA.size(); i++ ) {
		if ( contA.getElementDirect(i) != contB.getElementDirect(i) ) return false;
	}
	
	// for ( int i = 0; i < contA.size(); i++ ) {
		// SimpleVector<int> sA = subContA.getElementDirect(i);
		// SimpleVector<int> sB = subContB.getElementDirect(i);
		// if ( sA.size() != sB.size() ) return false;
		// for ( int j = 0; j < sA.size(); j++ ) {
			// if ( sA.getElementDirect(j) != sB.getElementDirect(j) ) return false;
		// }
	// }
	
	return true;
	
}

bool LunarMod::objIdReverseAction( int objId ) {
	if (objId <= 0) return false;

	bool r = false;
	if ( dirtyHoldingID <= 0 ) {
		switch (objId) {
			case 253: // full berry clay bowl
			case 225: // wheat bundle
				return true;
				break;
		}
		if ( getObject(objId) ) {
			char* descr	= getObject(objId)->description;
			if ( strstr(descr, "Bowl of") != NULL ) {
				return true;
			}
		}
	}
	return r;
}

bool LunarMod::objIdReverseActionServer( int objId ) {
	if (objId <= 0) return false;
	ObjectRecord* o = getObject(objId);
	if ( dirtyHoldingID <= 0 ) {
		if ( o->numSlots == 0 && !o->permanent ) {
			TransRecord *bareHandTrans = getTrans( 0, objId );
			if( bareHandTrans != NULL && bareHandTrans->newTarget != 0 ) {
				return true;
			}
		}
	}
	return false;
}

void LunarMod::actionAlphaRelativeToMe( int x, int y ) {
	
	if ( livingLifePage->lunarGetNextActionMessage() != NULL ) return;
	
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;
	
	int mapI = livingLifePage->hetuwGetMapI( x, y );

	int objId = getObjIdFromDirtyMapI(mapI);
	bool use = false;
	
	if (objId > 0) use = true;
	else use = false;

	if( dirtyHoldingID > 0 ) {
		ObjectRecord *held = getObject( dirtyHoldingID );

		if( held->foodValue == 0 ) {
			TransRecord *r = getTrans( dirtyHoldingID, -1 );
			if( r != NULL && r->newTarget != 0 ) { // a use-on-ground transition exists!
                use = true;	// override the drop action
			}
		}
	}

	bool remove = false;
	if (objIdReverseAction(objId)) remove = true;
	
	if ( ourLiveObject->holdingID < 0 ) { // holding babay
		remove = false;
		use = false;
	}

	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	if (remove) sprintf( msg, "REMV %d %d -1#", x, y);
	else if (use) sprintf( msg, "USE %d %d#", x, y);
	else sprintf( msg, "DROP %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	
	updateDirtyVars( false, x, y, false );
	
}

void LunarMod::actionBetaRelativeToMe( int x, int y ) {
	
	if ( livingLifePage->lunarGetNextActionMessage() != NULL ) return;
	
	bool modClick = true;
	
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;
	
	int mapI = livingLifePage->hetuwGetMapI( x, y );

	bool remove = false;
	if (dirtyHoldingID <= 0) {
		remove = true;
	}
	bool use = false;
	int objId = getObjIdFromDirtyMapI(mapI);
	
	ObjectRecord* heldObj = getObject(dirtyHoldingID);
	ObjectRecord* groundObj = getObject(objId);
	if ( dirtyHoldingID > 0 && objId > 0 ) {
		TransRecord *r = getTrans( dirtyHoldingID, objId );
		if( r == NULL &&
			groundObj->numSlots > 0 &&
			heldObj->containable &&
			heldObj->containSize <= groundObj->slotSize
			) {
		
			if ( !isShiftKeyDown() ) {
				if (mMapContainedStacks[mapI].size() > 0 && 
					mMapContainedStacks[mapI].size() < groundObj->numSlots ) {
					use = true;
					modClick = false;
				}
			}
		}
	} else if ( dirtyHoldingID > 0 && objId == 0 && heldObj->permanent ) {
		TransRecord *r = getTrans( dirtyHoldingID, -1 );
		if ( r != NULL && heldObj->foodValue == 0 && r->newTarget != 0 ) {
			use = true;
		}
	}
	
	if (objId > 0) {
		ObjectRecord* obj = getObject(objId);
		if (obj->numSlots == 0 && obj->blocksWalking) {
			TransRecord *r = getTrans( dirtyHoldingID, objId );
			if ( r != NULL && r->newTarget != 0 ) {
				use = true;
			}
		}
	}

	if ( objIdReverseAction( objId ) ) use = true;

	if ( ourLiveObject->holdingID < 0 ) { // holding babay
		remove = false;
		use = false;
	}

	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	if (use) sprintf( msg, "USE %d %d#", x, y);
	else if (remove) sprintf( msg, "REMV %d %d -1#", x, y);
	else sprintf( msg, "DROP %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	if (!remove) livingLifePage->hetuwSetNextActionDropping( true );
	
	updateDirtyVars( modClick, x, y, false );
	
}

void LunarMod::useBackpack(bool replace) {
	
	if ( livingLifePage->lunarGetNextActionMessage() != NULL ) return;
	
	// if ( ourLiveObject->inMotion ) {
		// dirtyUpdateDelay = true;
	// }
	
	int clothingSlot = 5; // backpack clothing slot

	int x = round( ourLiveObject->xd );
	int y = round( ourLiveObject->yd );
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);

	if( dirtyHoldingID > 0 ) {
		char msg[32];
		if (replace) {
			sprintf( msg, "DROP %d %d %d#", x, y, clothingSlot );
		} else {
			sprintf( msg, "SELF %d %d %d#", x, y, clothingSlot );
		}
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
		livingLifePage->hetuwSetNextActionDropping(true);
	} else {
		char msg[32];
		sprintf( msg, "SREMV %d %d %d %d#", x, y, clothingSlot, -1 );
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	}
	
	updateDirtyVarsUseClothing( clothingSlot, replace );
	
}

void LunarMod::useOnSelf() {
	
	if( dirtyHoldingID <= 0 ) return;
	
	if ( livingLifePage->lunarGetNextActionMessage() != NULL ) return;
	
	// if ( ourLiveObject->inMotion ) {
		// dirtyUpdateDelay = true;
	// }
	
	int x = round( ourLiveObject->xd );
	int y = round( ourLiveObject->yd );
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	
	char msg[32];
	sprintf( msg, "SELF %d %d %d#", x, y, -1 );
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );

	if( getObject( dirtyHoldingID )->foodValue > 0)
		livingLifePage->hetuwSetNextActionEating(true);
	
	updateDirtyVarsUseOnSelf( -1 );
	
}

void LunarMod::updateDirtyVars( char modClick, int x, int y, bool vanilla ) {
	
	int mapI = livingLifePage->hetuwGetMapI( x, y );
    if( mapI < 0 || mapI >= mMapD*mMapD ) return;
	
	ObjectRecord* groundObj = getObject(dirtyMap[mapI]);
	ObjectRecord* heldObj = getObject(dirtyHoldingID);
	
	if ( objIdReverseAction( dirtyMap[mapI] ) && !vanilla ) modClick = !modClick;
	if ( objIdReverseActionServer( dirtyMap[mapI] ) && vanilla ) modClick = !modClick;
	
	if ( modClick ) {
		
		if ( dirtyMap[mapI] == 0 ) {
			if ( dirtyHoldingID > 0 ) {
				TransRecord *r = getTrans( dirtyHoldingID, -1 );
				bool haveBarehandTrans = r != NULL && heldObj->foodValue == 0 && r->newTarget != 0;
				if (  haveBarehandTrans && (vanilla || heldObj->permanent) ) {
					dirtyHoldingID = r->newActor;
					dirtyMap[mapI] = r->newTarget;
				} else {
					dirtyMap[mapI] = dirtyHoldingID;
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
					
				}
			}
		} else {
			if ( dirtyHoldingID < 0 ) {
				dirtyHoldingID = 0;
			} else if ( dirtyHoldingID == 0 ) {
				if ( mMapContainedStacks[mapI].size() > 0 ) {
					dirtyHoldingID = mMapContainedStacks[mapI].getElementDirect( mMapContainedStacks[mapI].size() - 1 );
					dirtyHoldingStack = mMapSubContainedStacks[mapI].getElementDirect( mMapContainedStacks[mapI].size() - 1 );
				} else if ( !groundObj->permanent ) {
					dirtyHoldingID = dirtyMap[mapI];
					dirtyHoldingStack = mMapContainedStacks[mapI];
					dirtyMap[mapI] = 0;
				} else {
					TransRecord *r = getTrans( 0, dirtyMap[mapI] );
					if ( r != NULL && r->newTarget > 0 ) {
						dirtyHoldingID = r->newActor;
						dirtyMap[mapI] = r->newTarget;
					}
				}
			} else {
				TransRecord *r = getTrans( dirtyHoldingID, dirtyMap[mapI] );
				if( r == NULL &&
					groundObj->numSlots > 0 &&
					heldObj->containable &&
					heldObj->containSize <= groundObj->slotSize
					) {
					
					bool swapOrDrop = 0;
					
					bool diffFound = false;
					int diffIndex = 0;
					for ( int i = 0; i < mMapContainedStacks[mapI].size(); i++ ) {
						int contId = mMapContainedStacks[mapI].getElementDirect( i );
						ObjectRecord* contObj = getObject(contId);
						
						bool same = 
							isSameObject( 
								heldObj, contObj, 
								dirtyHoldingStack, mMapSubContainedStacks[mapI].getElementDirect( i )
								);
						
						if ( !same ) {
							diffFound = true;
							diffIndex = i;
							break;
						}
					}
					
					if ( mMapContainedStacks[mapI].size() == 0 ) {
						swapOrDrop = 1;
					} else if ( mMapContainedStacks[mapI].size() < groundObj->numSlots ) {
						if ( diffFound ) {
							swapOrDrop = 0;
						} else {
							swapOrDrop = 1;
						}
					} else {
						swapOrDrop = 0;
					}
					
					if ( swapOrDrop == 0 ) {
						dirtyHoldingID = mMapContainedStacks[mapI].getElementDirect( diffIndex );
						dirtyHoldingStack = mMapSubContainedStacks[mapI].getElementDirect( diffIndex );
					} else {
						dirtyHoldingID = 0;
						dirtyHoldingStack.deleteAll();
					}
					
				} else if ( 
					!groundObj->permanent && 
					!isSameObject( 
						heldObj, groundObj,
						dirtyHoldingStack, mMapContainedStacks[mapI] )
					) {
					TransRecord *t = getTrans( dirtyHoldingID, -1 );
					if ( !heldObj->permanent ) {
						int oldGroundID = dirtyMap[mapI];
						dirtyMap[mapI] = dirtyHoldingID;
						dirtyHoldingID = oldGroundID;
						dirtyHoldingStack = mMapContainedStacks[mapI];
					} else if ( t != NULL && heldObj->foodValue == 0 && t->newTarget != 0 ) {
						dirtyHoldingID = dirtyMap[mapI];
						dirtyMap[mapI] = t->newTarget;
						dirtyHoldingStack = mMapContainedStacks[mapI];
					}
				} else if ( r != NULL && dirtyHoldingStack.size() == 0 ) {
					dirtyHoldingID = r->newActor;
					dirtyMap[mapI] = r->newTarget;
				}
			}
		}
		
	} else {

		if( dirtyHoldingID < 0 ) {
			dirtyHoldingID = 0;
		} else if ( dirtyHoldingID == 0 ) {
			TransRecord *r = getTrans( 0, dirtyMap[mapI] );
			if( r != NULL ) {
				dirtyHoldingID = r->newActor;
				dirtyMap[mapI] = r->newTarget;
			} else {
				if ( groundObj != NULL ) {
					if ( !groundObj->permanent ) {
						dirtyHoldingID = dirtyMap[mapI];
						dirtyHoldingStack = mMapContainedStacks[mapI];
						dirtyMap[mapI] = 0;
					} else if ( mMapContainedStacks[mapI].size() > 0 ) {
						dirtyHoldingID = 
							mMapContainedStacks[mapI].getElementDirect( mMapContainedStacks[mapI].size() - 1 );
						dirtyHoldingStack = 
							mMapSubContainedStacks[mapI].getElementDirect( mMapContainedStacks[mapI].size() - 1 );
					}
				}
			}
		} else if ( dirtyHoldingID > 0 ) {
			if ( dirtyMap[mapI] == 0 ) {
				TransRecord *r = getTrans( dirtyHoldingID, -1 );
				if ( r != NULL && heldObj->foodValue == 0 && r->newTarget != 0 ) {
					dirtyHoldingID = r->newActor;
					dirtyMap[mapI] = r->newTarget;
				} else if ( !heldObj->permanent ) {
					dirtyMap[mapI] = dirtyHoldingID;
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				}
			} else {
				TransRecord *r = getTrans( dirtyHoldingID, dirtyMap[mapI] );
				if ( r == NULL || dirtyHoldingStack.size() > 0 ) {
					if( heldObj->containable &&
						heldObj->containSize <= groundObj->slotSize &&
						mMapContainedStacks[mapI].size() < groundObj->numSlots ) {
						dirtyHoldingID = 0;
						dirtyHoldingStack.deleteAll();
					}
				} else { 
					ObjectRecord* newTarget = getObject( r->newTarget );
					if ( newTarget == NULL || 
						( newTarget != NULL && newTarget->numSlots >= mMapContainedStacks[mapI].size() )
						) {
						int newActor = r->newActor;
						if ( getDummyParent( dirtyHoldingID ) == getDummyParent( newActor ) ) {
							newActor = dirtyHoldingID;
						}
						dirtyHoldingID = newActor;
						dirtyMap[mapI] = r->newTarget;
					} else if( heldObj->containable &&
								heldObj->containSize <= groundObj->slotSize &&
								mMapContainedStacks[mapI].size() < groundObj->numSlots ) {
								dirtyHoldingID = 0;
					}
				}
			}
		}
		
	}
	
	dirtyHoldingID = parseOneSecDecay(dirtyHoldingID);
	dirtyMap[mapI] = parseOneSecDecay(dirtyMap[mapI]);
	
	ObjectRecord* newGroundObj = getObject(dirtyMap[mapI]);
	if ( newGroundObj != NULL && newGroundObj->floor ) dirtyMap[mapI] = 0;
	
}

void LunarMod::updateDirtyVarsUseClothing( int clothingIndex, bool replace ) {

	ObjectRecord* heldObj = getObject(dirtyHoldingID);
	ObjectRecord *targetClothing = NULL;
	
	if ( clothingIndex == 0 ) targetClothing = ourLiveObject->clothing.hat;
	if ( clothingIndex == 1 ) targetClothing = ourLiveObject->clothing.tunic;
	if ( clothingIndex == 2 ) targetClothing = ourLiveObject->clothing.frontShoe;
	if ( clothingIndex == 3 ) targetClothing = ourLiveObject->clothing.backShoe;
	if ( clothingIndex == 4 ) targetClothing = ourLiveObject->clothing.bottom;
	if ( clothingIndex == 5 ) targetClothing = ourLiveObject->clothing.backpack;
	
	SimpleVector<int> clothingStack = ourLiveObject->clothingContained[ clothingIndex ];
	if( dirtyHoldingID > 0 ) {
		if ( targetClothing != NULL ) {
			TransRecord *r = getTrans( dirtyHoldingID, targetClothing->id );
			if( heldObj->containable &&
				heldObj->containSize <= targetClothing->slotSize ) {
				if (replace && clothingStack.size() > 0) {
					bool diffFound = false;
					int diffIndex = 0;
					for ( int i = 0; i < clothingStack.size(); i++ ) {
						int contId = clothingStack.getElementDirect( i );
						if ( contId != dirtyHoldingID ) {
							diffFound = true;
							diffIndex = i;
							break;
						}
					}
					if ( diffFound ) {
						dirtyHoldingID = clothingStack.getElementDirect( diffIndex );
					}
				} else {
					if ( clothingStack.size() < targetClothing->numSlots ) {
						dirtyHoldingID = 0;
					} else {
						updateDirtyVarsUseOnSelf( -1 );
					}
				}
			} else if( r != NULL && !replace ) {
				dirtyHoldingID = r->newActor;
			} else {
				
				updateDirtyVarsUseOnSelf( -1 );
				
			}
		} else {
			if (!replace) updateDirtyVarsUseOnSelf( -1 );
		}
	} else {
		if ( targetClothing != NULL ) {
			if ( clothingStack.size() > 0 ) {
				dirtyHoldingID = clothingStack.getElementDirect( clothingStack.size() - 1 );
			} else {
				dirtyHoldingID = targetClothing->id;
				dirtyHoldingStack = ourLiveObject->clothingContained[ clothingIndex ];
			}
		} 
	}	
	
}

void LunarMod::updateDirtyVarsUseOnSelf( int clothingIndex ) {
	
	ObjectRecord* heldObj = getObject(dirtyHoldingID);
	if ( clothingIndex == -1 ) {
		if ( heldObj == NULL ) {
		} else if ( heldObj->foodValue > 0 && ourLiveObject->foodStore < ourLiveObject->foodCapacity ) {
			TransRecord *r = getTrans( dirtyHoldingID, -1 );
			if ( r != NULL ) {
				dirtyHoldingID = r->newActor;
			} else {
				dirtyHoldingID = 0;
			}
		} else {
			if ( heldObj->clothing == 's' ) {
				if( ourLiveObject->clothing.frontShoe == NULL || 
					ourLiveObject->clothing.backShoe == NULL ) {
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				} else if ( dirtyHoldingID != ourLiveObject->clothing.frontShoe->id ) {
					dirtyHoldingID = ourLiveObject->clothing.frontShoe->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 2 ];
				} else if ( dirtyHoldingID != ourLiveObject->clothing.backShoe->id ) {
					dirtyHoldingID = ourLiveObject->clothing.backShoe->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 3 ];
				}
			} else if ( heldObj->clothing == 't' ) {
				if ( ourLiveObject->clothing.tunic == NULL ) {
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				} else {
					dirtyHoldingID = ourLiveObject->clothing.tunic->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 1 ];
				}
			} else if ( heldObj->clothing == 'h' ) {
				if ( ourLiveObject->clothing.hat == NULL ) {
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				} else {
					dirtyHoldingID = ourLiveObject->clothing.hat->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 0 ];
				}
			} else if ( heldObj->clothing == 'b' ) {
				if ( ourLiveObject->clothing.bottom == NULL ) {
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				} else {
					dirtyHoldingID = ourLiveObject->clothing.bottom->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 4 ];
				}
			} else if ( heldObj->clothing == 'p' ) {
				if ( ourLiveObject->clothing.backpack == NULL ) {
					dirtyHoldingID = 0;
					dirtyHoldingStack.deleteAll();
				} else {
					dirtyHoldingID = ourLiveObject->clothing.backpack->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ 5 ];
				}
			}
		}
	} else {
		
		ObjectRecord *targetClothing = NULL;
		if ( clothingIndex == 0 ) targetClothing = ourLiveObject->clothing.hat;
		if ( clothingIndex == 1 ) targetClothing = ourLiveObject->clothing.tunic;
		if ( clothingIndex == 2 ) targetClothing = ourLiveObject->clothing.frontShoe;
		if ( clothingIndex == 3 ) targetClothing = ourLiveObject->clothing.backShoe;
		if ( clothingIndex == 4 ) targetClothing = ourLiveObject->clothing.bottom;
		if ( clothingIndex == 5 ) targetClothing = ourLiveObject->clothing.backpack;
		
		if ( dirtyHoldingID == 0 ) {
			if ( targetClothing != NULL ) {
				TransRecord *r = getTrans( 0, targetClothing->id );
				if ( r != NULL ) {
					dirtyHoldingID = r->newActor;
				} else {
					dirtyHoldingID = targetClothing->id;
					dirtyHoldingStack = ourLiveObject->clothingContained[ clothingIndex ];
				}
			}
		} else {
			TransRecord *r = getTrans( dirtyHoldingID, targetClothing->id );
			if( heldObj->containable &&
				heldObj->containSize <= targetClothing->slotSize &&
				ourLiveObject->clothingContained[ clothingIndex ].size() < targetClothing->numSlots
				) {
				dirtyHoldingID = 0;
			} else if ( heldObj->clothing != 'n' ) {
				updateDirtyVarsUseOnSelf( -1 );
			} else if ( r != NULL ) {
				dirtyHoldingID = r->newActor;
			}
		}
	}
	
}

void LunarMod::refreshDirtyVars() {
    if( ourLiveObject == NULL ) return;
	dirtyHoldingID = ourLiveObject->holdingID;
	refreshDirtyMap();
	
	lastMapOffsetX = livingLifePage->mMapOffsetX;
	lastMapOffsetY = livingLifePage->mMapOffsetY;
}

int LunarMod::getObjIdFromDirtyMapI( int inMapI ) {
	if( inMapI < 0 || inMapI >= mMapD*mMapD ) return -1;
    return dirtyMap[inMapI];
}


void LunarMod::drawPoint(doublePair posCen, string color) {
	float pointSize = 3 * HetuwMod::guiScale;
	
	if (color == "red") setDrawColor( 1, 0, 0, 1 );
	if (color == "green") setDrawColor( 0, 1, 0, 1 );
	if (color == "blue") setDrawColor( 0, 0, 1, 1 );
	
	drawRect( posCen, pointSize, pointSize );	
}

void LunarMod::drawStr(
	string str,
	doublePair pos,
	string anchorPos,
	string font,
	float fontScale,
	// TextAlignment align,
	bool withBackground,
	bool avoidOffScreen
	) {
	
	//String split by newline
	string delimiter = "\n";
	vector<string> lines;
	string::size_type curr = 0;
	string::size_type prev = 0;
	while ((curr = str.find(delimiter, prev)) != string::npos)
	{
		lines.push_back(str.substr(prev, curr - prev));
		prev = curr + 1;
	}
	lines.push_back(str.substr(prev));
	
	//Get longest textwidth among lines
	float textWidth = 0;
	for (unsigned int i=0;i<lines.size();i++) {
		float textWidth_temp = 0;
		if (font == "handwritten") {
			textWidth_temp = livingLifePage->hetuwMeasureScaledHandwritingFont( lines[i].c_str(), fontScale );
		} else if (font == "main") {
			textWidth_temp = livingLifePage->hetuwMeasureScaledMainFont( lines[i].c_str(), fontScale );
		}
		if (textWidth_temp > textWidth) textWidth = textWidth_temp;
	}
	
	//Getting TL and MC pos from anchor
	int numOfLines = lines.size();
	float lineHeight = 24*fontScale;
	float paddingX = 6*fontScale;
	float paddingY = 6*fontScale;
	float recWidth = textWidth + paddingX*2;
	float recHeight = numOfLines * lineHeight + paddingY*2;
	
	doublePair posTL;
	doublePair posMC;
	if (anchorPos == "TL") {
		posTL = pos;
		posMC = {posTL.x + recWidth/2, posTL.y - recHeight/2};
	} else if (anchorPos == "MC") {
		posMC = pos;
		posTL = {posMC.x - recWidth/2, posMC.y + recHeight/2};
	} else if (anchorPos == "BR") {
		posMC = {pos.x - recWidth/2, pos.y + recHeight/2};
		posTL = {posMC.x - recWidth/2, posMC.y + recHeight/2};
	}
	
	//Getting offset to stay on screen
	doublePair screenCenter = livingLifePage->hetuwGetLastScreenViewCenter();
	float viewWidth = HetuwMod::viewWidth;
	float viewHeight = HetuwMod::viewHeight;
	
	if (avoidOffScreen) {
		doublePair offset = {0.0, 0.0};
		if (posTL.x + recWidth > screenCenter.x + viewWidth/2) {
			offset.x = screenCenter.x + viewWidth/2 - recWidth - posTL.x;
		} else if (posTL.x < screenCenter.x - viewWidth/2) {
			offset.x = - posTL.x + screenCenter.x - viewWidth/2;
		}
		if (posTL.y > screenCenter.y + viewHeight/2) {
			offset.y = screenCenter.y + viewHeight/2 - posTL.y;
		} else if (posTL.y - recHeight < screenCenter.y - viewHeight/2) {
			offset.y = recHeight - posTL.y + screenCenter.y - viewHeight/2;
		}
		posTL = add(posTL, offset);
		posMC = add(posMC, offset);
	}
	
	//Drawing background
	if (withBackground) {
		setDrawColor( 0, 0, 0, 0.8 );
		drawRect( posMC, recWidth/2, recHeight/2);
	}
	
	
	//Drawing the lines
	doublePair posLineTL = {posTL.x + paddingX, posTL.y - paddingY - lineHeight/2};
	for (unsigned int i=0;i<lines.size();i++) {

		setDrawColor( 1, 1, 1, 1 );
		if (font == "handwritten") {
			livingLifePage->hetuwDrawScaledHandwritingFont( lines[i].c_str(), posLineTL, fontScale, alignLeft );
		} else if (font == "main") {
			livingLifePage->hetuwDrawScaledMainFont( lines[i].c_str(), posLineTL, fontScale, alignLeft );
		}
		
		posLineTL.y -= lineHeight;
	}
}

void LunarMod::drawTileRect( int x, int y, string color, bool flashing ) {
	doublePair pos = { (double)x, (double)y };
	pos.x *= CELL_D;
	pos.y *= CELL_D;
	float maxAlpha = 0.5;
	float alpha;
	if (flashing) {
		alpha = (HetuwMod::stepCount % 40) / 40.0;
		if (alpha > 0.5) alpha = 1 - alpha;
		alpha *= maxAlpha;
	} else {
		alpha = maxAlpha;
	}
	if (color == "red") setDrawColor( 1, 0, 0, alpha );
	if (color == "green") setDrawColor( 0, 1, 0, alpha );
	if (color == "blue") setDrawColor( 0, 0, 1, alpha );
	drawRect( pos, CELL_D/2, CELL_D/2 );
}

void LunarMod::drawPath( GridPos* path, int pathLength ) {
	if (path == NULL) return;
	for (int i = 0; i < pathLength-1; i++) {
		drawTileRect( path[i].x, path[i].y, "blue", false );
	}
	drawTileRect( path[pathLength-1].x, path[pathLength-1].y, "green", false );
}






void LunarMod::livingLifeDraw(float worldMouseX, float worldMouseY) {
	
	//TEST
	// drawPath( testPath, testCount );
	// drawTileRect( testTile.x, testTile.y, "red", true );
	// for (int i=0; i< testTiles.size(); i++) {
		// drawTileRect( testTiles[i].x, testTiles[i].y, "red", true );
	// }

	if (showActiveMods) {
		
		// doublePair screenCenter = livingLifePage->hetuwGetLastScreenViewCenter();
		doublePair mouseScreenPos = {worldMouseX, worldMouseY};
		mouseX = int(round( worldMouseX / (float)CELL_D ));
		mouseY = int(round( worldMouseY / (float)CELL_D ));
		// GridPos mousePos = { mouseX, mouseY };
		
		int objId = livingLifePage->hetuwGetObjId( mouseX, mouseY );
		// ObjectRecord* o = getObject(objId);
		// int victimID = getClosestPlayerID();
		// LiveObject *victim = livingLifePage->getLiveObject(victimID);
		
		int mapI = livingLifePage->hetuwGetMapI( mouseX, mouseY );
		// int posBiome = livingLifePage->mMapBiomes[mapI];
		
		// int holdingID = ourLiveObject->holdingID;
        
        int graveID = -1;
        for( int g=0; g<livingLifePage->mGraveInfo.size(); g++ ) {
            GraveInfo *gI = livingLifePage->mGraveInfo.getElement( g );
            if( gI->worldPos.x == mouseX && gI->worldPos.y == mouseY ) {
                graveID = gI->playerID;
            }
        }
		
		sprintf( d, 
			"\n HO: %d, %d, GRD: %d, %d"
			"\n STACK: %d, NULL: %d"
            "\n GRAVE: %d",
			ourLiveObject->holdingID, dirtyHoldingID, objId, getObjIdFromDirtyMapI(mapI),
			dirtyHoldingStack.size(), livingLifePage->lunarGetNextActionMessage() == NULL,
            graveID
		);
		
		char sBuf[64];
		sprintf( sBuf, "%d, %d, %d%s", 
			mouseX, mouseY, objId, d
		);
		
		float fontScale = HetuwMod::guiScale * 0.7;
		float paddingX = 20*HetuwMod::guiScale;
		float paddingY = 40*HetuwMod::guiScale;
		doublePair strPos = {mouseScreenPos.x + paddingX, mouseScreenPos.y - paddingY};
		drawStr( sBuf, strPos, "TL", "handwritten", fontScale);
		
	}
	
	if (showActiveMods) {
		drawPath( currentMoveStatus->autoMovePath, currentMoveStatus->autoMovePathLength );
	}
	
	if (showActiveMods) {
		if ( ourLiveObject->foodStore <= 8 && ourLiveObject->maxFoodCapacity > 8) {
			float alpha = ( 1 - (ourLiveObject->foodStore / 8.0) ) * 0.3;
			doublePair startPos = livingLifePage->hetuwGetLastScreenViewCenter();
			setDrawColor( 1, 0, 0, alpha );
			drawRect( startPos, HetuwMod::viewWidth * HetuwMod::guiScale, HetuwMod::viewHeight * HetuwMod::guiScale );
		}
	}
	
	if (showActiveMods) {
		
		float guiScale = HetuwMod::guiScale;
		
		vector<string> mods = {
			"TESTMODE",
			"EMOTSPAM", 
			"AUTOWALK",
			"AUTOKILL", 
			"AUTOFOLLOW", 
			"AUTOSEARCH", 
			"AUTOEAT", 
			"AUTOWALK", 
			"GHOST", 
			"GHOST (WAKING)"
			};
		int numMods = mods.size();
		bool modsStates[numMods] = {
			testMode,
			emoteSpamMode, 
			autoMode,
			killMode, 
			followMode, 
			searchMode, 
			afkMode, 
			returnMode, 
			ghostMode, 
			wakingMode
			};
		string info[numMods];
		int numModsOn = 0;
		string fullStr;
		for (int i=0; i<numMods; i++) {
			if (modsStates[i]) {
				numModsOn++;
				string mode = "";
				string details = "";
				
				if (i == 1) { //emoteSpamMode
					if (emoteSpamIndex == 0) details = "KEK";
					if (emoteSpamIndex == 1) details = "898989";
				} else if (i == 2) { //autoMode
					details = currentMoveStatus->automoveDirection;
				} else if (i == 7) { //returnMode
					details = "( " + to_string(int(savedX)) + ", " + to_string(int(savedY)) + " )";
				} else if (i == 3 || i == 4) {
					LiveObject *player = livingLifePage->getLiveObject(playerID);
					if (player != NULL) {
						string name = "";
						int id = player->id;
						if (player->name != NULL) {
							name = string(player->name);
						} else {
							name = "UNNAMED";
						}
						if (i == 3) {
							if (killModeIndex == 0) mode = "ANTIGRIEF";
							if (killModeIndex == 1) mode = "PVP";
						}
						if (i == 4) {
							if (followDistanceIndex == 0) mode = "STICKY";
							if (followDistanceIndex == 1) mode = "FOLLOW";
							if (followDistanceIndex == 2) mode = "STALK";
						}
						details = name + " (" + to_string(id) + ")";
					}
				} 
				else if (i == 5) {
					if (!autoMode && !returnMode && !followMode) {
						if (autoSearchStatus != NULL) details = to_string(autoSearchStatus->nextSearchDirection);
					}
					if ( !posEqual(currentMoveStatus->autoSearchReachPos, posNOTFOUND) ) {
						details += " ( " + to_string(currentMoveStatus->autoSearchReachPos.x) + ", " + to_string(currentMoveStatus->autoSearchReachPos.y) + " )";
					} else if (!autoMode && !returnMode && !followMode) {
						GridPos searchPos = getSpiralSearchPos(autoSearchStatus);
						details += " ( " + to_string(searchPos.x) + ", " + to_string(searchPos.y) + " )";
					}
				} 
				// else if (i == 6) {
					// if (!autoMode && !returnMode && !followMode && !searchMode) details = to_string(nextSearchDirection);
				// } else if (i == 8) {
					// details += " ( " + to_string(int(ourLiveObject->xServer)) + ", " + to_string(int(ourLiveObject->yServer)) + " )";
				// }
				
				if (details != "") details = " - " + details;
				if (mode != "") mode = " " + mode;
				info[i] = mods[i] + mode + details;
				fullStr += info[i] + "\n";
			}
		}
		
		if (numModsOn > 0) {
			fullStr.pop_back();
			
			doublePair screenCen = livingLifePage->hetuwGetLastScreenViewCenter();
			doublePair screenTL = {screenCen.x - HetuwMod::viewWidth/2, screenCen.y + HetuwMod::viewHeight/2};
			doublePair posTL = {screenTL.x + 20*guiScale +200*guiScale, screenTL.y - 20*guiScale};
			
			drawStr(fullStr, posTL, "TL", "handwritten", guiScale);
		}
	}
	
}

void LunarMod::livingLifeStep() {
	
	if ( vogMode ) {
		if ( followMode && !waitingForVogUpdate ) {
            
            int followDist = 3;
            if( followDistanceIndex == 0 ) followDist = 1;
			
			LiveObject *player = livingLifePage->getLiveObject(playerID);
			if (player == NULL) {
				playerID = 0;
				followMode = false;
				return;
			}

			doublePair vogPos = livingLifePage->hetuwGetVogPos();
			GridPos newPos = { (int)player->currentPos.x, (int)player->currentPos.y };
			double dist = distance( vogPos.x, vogPos.y, newPos.x, newPos.y );
			
			if ( dist > 100 ) {
				playerID = 0;
				followMode = false;
			} else if ( dist >= followDist ) {
				char *message = autoSprintf( "VOGM %d %d#", newPos.x, newPos.y );
				livingLifePage->sendToServerSocket( message );
				waitingForVogUpdate = true;
			}
		}
		
		return;
	}
	
 	ourLiveObject = livingLifePage->getOurLiveObject();
	if (!ourLiveObject) return;
	currentX = round(ourLiveObject->currentPos.x);
	currentY = round(ourLiveObject->currentPos.y);
	currentPos = {currentX, currentY};
	
	
	if (afkModeVerbose) { //BUG
		// HetuwMod::sayBuffer.clear();
		// HetuwMod::sayBuffer.shrink_to_fit();
		// HetuwMod::Say("AFK-MODE");
	}
	
	if (emoteSpamMode) {
		double curTime = Time::getCurrentTime();
		if (curTime - emoteSpamLastTime > 0.05) {
			if (emoteSpamIndex == 0) {
				vector<int> emotes = {0, 1, 2, 3, 4, 5, 6, 9, 12, 13, 14, 15};
				std::uniform_int_distribution<int> uni(0, 11);
				auto randInt = uni(rng);
				while (currentEmote == emotes[randInt]) {
					randInt = uni(rng);
				}
				currentEmote = emotes[randInt];
			} else if (emoteSpamIndex == 1) {
				if (currentEmote == 10) {
					currentEmote = 11;
				} else {
					currentEmote = 10;
				}
			}
			char message[64];
			sprintf( message, "EMOT 0 0 %i#", currentEmote);
			livingLifePage->sendToServerSocket( message );
			
			emoteSpamLastTime = Time::getCurrentTime();;
		}
	}
	
	if (wakingMode && HetuwMod::stepCount % 30 == 0) {
		if ( ghostModeAwake() ) {
			wakingMode = false;
		}
		return;
	}
	
	if (currentMoveStatus != NULL) {
		if (currentMoveStatus->waitForDoorToOpen) {
			int objId = livingLifePage->hetuwGetObjId( currentMoveStatus->lastDoorPos.x, currentMoveStatus->lastDoorPos.y );
			ObjectRecord* obj = getObject( objId );
			if ( isClosedDoor(obj) ) {
				return;
			} else {
				currentMoveStatus->waitForDoorToOpen = false;
			}
		}
		
		if (currentMoveStatus->waitingToDrop) {
			if( ourLiveObject->holdingID != 0 ) {
				return;
			} else {
				currentMoveStatus->waitingToDrop = false;
			}
		}
		
		if (currentMoveStatus->waitingToGet) {
			if( ourLiveObject->holdingID == 0 ) {
				return;
			} else {
				currentMoveStatus->waitingToGet = false;
			}
		}
		
		if (currentMoveStatus->waitingToUseOnSelf) {
			if( ourLiveObject->holdingID != 0 ) {
				return;
			} else {
				currentMoveStatus->waitingToUseOnSelf = false;
			}
		}
		
		if (currentMoveStatus->autoCraftDropMade) { //BUG
			GridPos emptyPos = getClosestTile(currentPos, 0);
			currentMoveStatus->autoCraftDropMadeResult = moveAndYeet(emptyPos);
			if (currentMoveStatus->autoCraftDropMadeResult == 0) return;
			if (currentMoveStatus->autoCraftDropMadeResult != 0) currentMoveStatus->autoCraftDropMade = false;
			return;
		}
		
		if (currentMoveStatus->droppingHorse) {
			currentMoveStatus->droppingHorse_result = dropHorse();
			if (currentMoveStatus->droppingHorse_result == 0) return;
			if (currentMoveStatus->droppingHorse_result != 0) currentMoveStatus->droppingHorse = false;
			return;
		}
		
		if (currentMoveStatus->gettingBackOnHorse) {
			if (getHorse() != 1) return; //horsePos is unreachable
			currentMoveStatus->targetPos = posNOTFOUND;
			currentMoveStatus->droppingHorse_result = 1;
			currentMoveStatus->gettingBackOnHorse = false;
			currentMoveStatus->wasOnHorse = false;
		}
	}
	
	int numPathMapCells = pathFindingD * pathFindingD;
	memset( bMap, false, numPathMapCells );
	updateBlockedMap(bMap);
	

	

	
	if (ghostMode) {
		doublePair screenViewCenter = { 
			double(ourLiveObject->currentPos.x * CELL_D), 
			double(ourLiveObject->currentPos.y * CELL_D) };
		livingLifePage->lunarSetLastScreenViewCenter( screenViewCenter );
		
		if ( autoMode || followMode || searchMode || returnMode ) {
		} else {
			ourLiveObject->currentSpeed = 0.85; //0.5 //0.85
		}
	}
	

	
	
	if (testMode) {
		// int objId = 82;
		// int objId = 253;
		// Make( objId );
		
		

		
		
		
		
		// int objId = 33;
		
		// GridPos o = {0, 0};
		// GridPos dropPos = getClosestTile(o, 0);
		// if ( posEqual(testTile, posNOTFOUND) ) testTile = getClosestTile(currentPos, objId, testTiles);
		// int stat = moveObj( testTile, dropPos, objId );
		// if (stat == 1) {
			// testTiles.push_back( dropPos );
			// testTile = posNOTFOUND;
		// }
		
		
		LiveObject *player = livingLifePage->getLiveObject(playerID);
		if (player == NULL) {
			playerID = 0;
			followMode = false;
			moveReset();
			return;
		}
		autoFollow(playerID);
		
		float age = livingLifePage->hetuwGetAge( player );
		
		if ( age >= 5.0 && ourLiveObject->holdingID != 0 ) {
			char buffer[32];
			string message = "%s %d %d %d %d#";
			string action = "UBABY";
			int x = player->xd;
			int y = player->yd;
			int extra = 0;
			int extra2 = player->id;
			bool fast = true;
			sprintf( buffer, message.c_str(), action.c_str(), livingLifePage->sendX(x), livingLifePage->sendY(y), extra, extra2);
			if (!fast) livingLifePage->hetuwSetNextActionMessage( buffer, x, y );
			if (fast) livingLifePage->sendToServerSocket( buffer );		
		} else if ( age < 5.0 && ourLiveObject->holdingID == 0 ) {
			char buffer[32];
			string message = "%s %d %d %d#";
			string action = "BABY";
			int x = player->xd;
			int y = player->yd;
			int extra = player->id;
			int extra2 = 0;
			bool fast = true;
			sprintf( buffer, message.c_str(), action.c_str(), livingLifePage->sendX(x), livingLifePage->sendY(y), extra, extra2);
			if (!fast) livingLifePage->hetuwSetNextActionMessage( buffer, x, y );
			if (fast) livingLifePage->sendToServerSocket( buffer );		
		}
		

		
		return;
	}

	
	
	
	
	if (afkMode && ourLiveObject->age > 3) {
		if ( killMode || followMode || autoMode || searchMode || returnMode ) {
			bool wasteFood = true;
			bool active = false;
			int status = autoEat(wasteFood, active);
			if (status) return;
		} else {
			bool wasteFood = false;
			bool active = true;
			autoEat(wasteFood, active);
			return;
		}
	}
	
	if (killMode) {
		if (autoKill(playerID)) {
			killMode = false;
			moveReset();
		}
	}
	
	if (followMode) {
		LiveObject *player = livingLifePage->getLiveObject(playerID);
		if (player == NULL) {
			playerID = 0;
			followMode = false;
			moveReset();
			return;
		}
		if (followDistanceIndex == 0) {
			autoFollow(playerID);
		} else if (followDistanceIndex == 1) {
			autoFollow(playerID, 3.0f);
		} else if (followDistanceIndex == 2) {
			autoFollow(playerID, 11.0f);
		}
		return;
	}
	
	if (autoMode) {
		if (searchMode) {
			if ( autoSearch(HetuwMod::objIsBeingSearched, foundPosHistory, false, true) != 0 ) return;
		} 
		autoMove();
		return;
	}
	
	if (returnMode) {
		if (searchMode) {
			if ( autoSearch(HetuwMod::objIsBeingSearched, foundPosHistory, false, true) != 0 ) return;
			
		} 
		autoReturn();
		return;
	}

	if (searchMode) {
		if (autoSearchStatus == NULL) autoSearchHistoryReset();
		autoSearch(HetuwMod::objIsBeingSearched, foundPosHistory, true, true);
		return;
	}

}
	
bool LunarMod::livingLifeKeyUp(unsigned char inASCII) {
	return false;
}
	
bool LunarMod::livingLifeKeyDown(unsigned char inASCII) {	
	
	if (
		livingLifePage->lunarGetNextActionMessage() == NULL &&
		(lastMapOffsetX != livingLifePage->mMapOffsetX ||
		lastMapOffsetY != livingLifePage->mMapOffsetY)
		) {
		refreshDirtyVars();
	}
	
	if (Phex::onKeyDown(inASCII)) return true;
	
	if (inASCII == 'V') {
		allModsOff();
		return false;
	}
	// if (inASCII == 'I') return false;
	
	bool commandKey = isCommandKeyDown();
	bool shiftKey = isShiftKeyDown();
	
	if ( vogMode ) {
		
		if ( livingLifePage->lunarIsVogPickerOn() ) return false;
        
        if (livingLifePage->hetuwSayFieldIsFocused()) return false;
        
        if (shiftKey && !commandKey && toupper(inASCII) == toupper(followKey)) { //shift + G
            followDistanceIndex = (followDistanceIndex + 1) % 3;
            return true;
        }
		
		if (!shiftKey && !commandKey && toupper(inASCII) == toupper(followKey)) { //G
			if (!followMode) {
				playerID = mouseOverPlayerID;
				LiveObject *player = livingLifePage->getLiveObject(playerID);
				if (player == NULL) {
					doublePair vogPos = livingLifePage->hetuwGetVogPos();
					playerID = getClosestPlayerID( (int)vogPos.x, (int)vogPos.y );
				}
				player = livingLifePage->getLiveObject(playerID);
				if ( player == NULL ) {
					playerID = 0;
					followMode = false;
					return true;
				}
                followDistanceIndex = 1;
				followMode = true;
				return true;
			} else {
				followMode = false;
				return true;
			}
		}
		
		return false;
	}
	
	if (livingLifePage->hetuwSayFieldIsFocused()) return false;
	if (HetuwMod::getSearchInput > 0) return false;
	if (HetuwMod::getCustomCords == 1) return false;
	if (HetuwMod::bNextCharForHome) return false;
	
	if (nextCharForDest) {
		nextCharForDest = false;
		for (unsigned i=0; i<HetuwMod::homePosStack.size(); i++) {
			if (toupper(inASCII) == HetuwMod::homePosStack[i]->c) {
				savedX = HetuwMod::homePosStack[i]->x;
				savedY = HetuwMod::homePosStack[i]->y;
				break;
			}
		}
		return true;
	}
	
	if (!shiftKey && (inASCII == slashDieKey || inASCII == toupper(slashDieKey))) { //M
		char *message = autoSprintf( "DIE 0 0#" );
		livingLifePage->sendToServerSocket( message );
		
		if (ourLiveObject->age > 2) {
			livingLifePage->lunarForceDisconnect( );
		}
	}
	
	if (!shiftKey && (inASCII == uiKey || inASCII == toupper(uiKey))) { //O
		showActiveMods = !showActiveMods;
		HetuwMod::bDrawHomeCords = showActiveMods;
		HetuwMod::bDrawCords = showActiveMods;
		if (showActiveMods) {
			HetuwMod::iDrawPlayersInRangePanel = 2;
		} else {
			HetuwMod::iDrawPlayersInRangePanel = 0;
		}
	}
	
	
	
	
	if (inASCII == 'l') {
		
		testMode = !testMode;

		return true;
	}
	
	
	
	// sprintf(d, ", %d / %c", inASCII, inASCII);
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(ghostKey)) { //N
		if (!ghostMode) {
			wakingMode = false;
			ghostMode = true;
		} else {
			allModsOff();
			wakingMode = true;
			ghostMode = false;
		}
		return true;
	}
	
	if (!shiftKey && !commandKey) {
		int doubleTapCountInv = 15;
		if (toupper(inASCII) == toupper(upKey)) {
			if (upCount >= 0 && abs(HetuwMod::stepCount - upCount) < doubleTapCountInv) {
				currentMoveStatus->automoveDirection += strUpKey;
				autoMode = true;
				return true;
			} else {
				autoMode = false;
			}
			upCount = HetuwMod::stepCount;
		}
		if (toupper(inASCII) == toupper(leftKey)) {
			if (leftCount >= 0 && abs(HetuwMod::stepCount - leftCount) < doubleTapCountInv) {
				currentMoveStatus->automoveDirection += strLeftKey;
				autoMode = true;
				return true;
			} else {
				autoMode = false;
			}
			leftCount = HetuwMod::stepCount;
		}
		if (toupper(inASCII) == toupper(downKey)) {
			if (downCount >= 0 && abs(HetuwMod::stepCount - downCount) < doubleTapCountInv) {
				currentMoveStatus->automoveDirection += strDownKey;
				autoMode = true;
				return true;
			} else {
				autoMode = false;
			}
			downCount = HetuwMod::stepCount;
		}
		if (toupper(inASCII) == toupper(rightKey)) {
			if (rightCount >= 0 && abs(HetuwMod::stepCount - rightCount) < doubleTapCountInv) {
				currentMoveStatus->automoveDirection += strRightKey;
				autoMode = true;
				return true;
			} else {
				autoMode = false;
			}
			rightCount = HetuwMod::stepCount;
		}
	}
	
	
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(afkKey)) { //shift + V
		afkModeVerbose = !afkModeVerbose;
		return true;
	}
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(afkKey)) { //V
 		if (!afkMode) {
			afkMode = true;
		} else {
			afkMode = false;
		}
		return true;
	}
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(killKey)) { //T
		playerID = mouseOverPlayerID;
		LiveObject *player = livingLifePage->getLiveObject(playerID);
		if (player == NULL) playerID = getClosestPlayerID();
		player = livingLifePage->getLiveObject(playerID);
		if ( player == NULL || killMode ) {
			killMode = false;
			moveReset();
		} else if ( !killMode ) {
			moveReset();
			autoMode = false;
			returnMode = false;
			searchMode = false;
			followMode = false;
			killMode = true;
		}
		return true;
	}
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(killKey)) { //shift + T
		killModeIndex = (killModeIndex + 1) % 3;
		return true;
	}
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(returnKey)) { //Z
 		if (!returnMode) {
			allModsOff();
			returnMode = true;
		} else {
			moveReset();
			returnMode = false;
		}
		return true;
	}
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(returnKey)) { //shift + Z
		nextCharForDest = true;
		return true;
	}
	
	if (inASCII + 64 == toupper(returnKey)) { //Ctrl + Z
		savedX = mouseX;
		savedY = mouseY;
		return true;
	}
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(searchKey)) { //shift + R
		autoSearchHistoryReset();
		return true;
	}

	if (inASCII + 64 == toupper(searchKey)) { //Ctrl + R
		autoSearchContinue();
		return true;
	}	
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(searchKey)) { //R
 		if (!searchMode) {
			if (!autoMode && !returnMode) {
				allModsOff(); //BUG
			}
			searchMode = true;
		} else {
			searchMode = false;
		}
		return true;
	} 
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(followKey)) { //shift + G
		followDistanceIndex = (followDistanceIndex + 1) % 3;
		return true;
	}
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(followKey)) { //G
		playerID = mouseOverPlayerID;
		LiveObject *player = livingLifePage->getLiveObject(playerID);
		if (player == NULL) playerID = getClosestPlayerID();
		player = livingLifePage->getLiveObject(playerID);
		if ( player == NULL || followMode ) {
			followMode = false;
			moveReset();
		} else if ( !followMode ) {
			moveReset();
			autoMode = false;
			returnMode = false;
			searchMode = false;
			killMode = false;
			followMode = true;
		}
		return true;
	}
	
	if (!shiftKey && !commandKey && toupper(inASCII) == toupper(emoteKey)) { //I
		emoteSpamMode = !emoteSpamMode;
		return true;
	}
	
	if (shiftKey && !commandKey && toupper(inASCII) == toupper(emoteKey)) { //shift + I
		emoteSpamIndex = (emoteSpamIndex + 1) % 2;
		return true;
	}
	
	if (toupper(inASCII) == toupper(upKey) ||
		toupper(inASCII) == toupper(leftKey) ||
		toupper(inASCII) == toupper(downKey) ||
		toupper(inASCII) == toupper(rightKey) ) {
		
		allModsOff();
	}
	
	return false;
}

bool LunarMod::livingLifePageMouseDown(float inX, float inY) {
	refreshDirtyVars();
	
	return false;
}




// sprintf( msg, "USE %d %d#", x, y);
// sprintf( msg, "DROP %d %d -1#", x, y);
// sprintf( msg, "REMV %d %d -1#", x, y);
// sprintf( msg, "SELF %d %d %d#", x, y, -1 );
// sprintf( msg, "SREMV %d %d %d %d#", x, y, clothingIndex, slotIndex );
void LunarMod::sendAction(string action, int x, int y, int extra, int extra2, bool fast) {
	char buffer[32];
	string message = "%s %d %d %d#";
	if (action == "USE") message = "%s %d %d#";
	// if (action != "SELF") extra = -1;
	if (action == "SREMV") message = "%s %d %d %d %d#";
	sprintf( buffer, message.c_str(), action.c_str(), livingLifePage->sendX(x), livingLifePage->sendY(y), extra, extra2);
	if (!fast) livingLifePage->hetuwSetNextActionMessage( buffer, x, y );
	if (fast) livingLifePage->sendToServerSocket( buffer );
}

void LunarMod::sendMove(GridPos* path, int pathLength) {
	SimpleVector<char> moveMessageBuffer;
	
	moveMessageBuffer.appendElementString( "MOVE" );
	ourLiveObject->lastMoveSequenceNumber ++;
	
	// start is absolute
	char *startString = 
		autoSprintf( " %d %d @%d", 
					livingLifePage->sendX( path[0].x ),
					livingLifePage->sendY( path[0].y ),
					ourLiveObject->lastMoveSequenceNumber );
	moveMessageBuffer.appendElementString( startString );
	delete [] startString;
	
	for( int i=1; i<pathLength; i++ ) {
		// rest are relative to start
		char *stepString = autoSprintf( " %d %d", 
										path[i].x - path[0].x,
										path[i].y - path[0].y );
		moveMessageBuffer.appendElementString( stepString );
		delete [] stepString;
	}
	moveMessageBuffer.appendElementString( "#" );
	
	char *message = moveMessageBuffer.getElementString();
	livingLifePage->sendToServerSocket( message );
	delete [] message;
}

void LunarMod::sendStep(int startX, int startY, int relativeX, int relativeY) {
	
	ourLiveObject->lastMoveSequenceNumber ++;
	char *message = 
		autoSprintf( "MOVE %d %d @%d %d %d#", 
					livingLifePage->sendX( startX ),
					livingLifePage->sendY( startY ),
					ourLiveObject->lastMoveSequenceNumber,
					relativeX,
					relativeY );

	livingLifePage->sendToServerSocket( message );
    delete [] message;
	
}





void LunarMod::moveReset() {
	
	if (currentMoveStatus != NULL) delete currentMoveStatus;
	currentMoveStatus = new moveStatus();
	
}

//return 1 if arrived, 0 if not, -1 when arrived at closestFound
int LunarMod::move(int destX, int destY) {
	
	GridPos dest = {destX, destY};
	
	if ( posEqual(currentPos, dest) ) {
		return 1;
	}
	
	if (ourLiveObject->heldByAdultID != -1) return 0;
	
	// ourLiveObject->shouldDrawPathMarks = true;
	HetuwMod::stopAutoRoadRun = true;

	GridPos* path_old = ourLiveObject->pathToDest;
	if (path_old != NULL) {
		int pathLength_old = ourLiveObject->pathLength;
		GridPos tile1 = path_old[1];
		GridPos lastTar = path_old[pathLength_old-1];
		bool tile1Dangerous = !HetuwMod::tileHasNoDangerousAnimals(tile1.x, tile1.y);
		if ( !(currentX == lastTar.x && currentY == lastTar.y) && !tile1Dangerous ) {
			return 0;
		}
	}

	int goalX = destX;
	int goalY = destY;
	if (destX - currentX > pathFindingD) goalX = currentX + pathFindingD;
	if (destY - currentY > pathFindingD) goalY = currentY + pathFindingD;
	if (destX - currentX < -pathFindingD) goalX = currentX - pathFindingD;
	if (destY - currentY < -pathFindingD) goalY = currentY - pathFindingD;
	
	if (ghostMode) {
		
		bool destUnknown = true;
		
		int objId = livingLifePage->hetuwGetObjId( goalX, goalY );
		destUnknown = objId < 0 || objId > maxObjects;
		
		while (destUnknown) {
			if (goalX > currentX) goalX--;
			if (goalX < currentX) goalX++;
			if (goalY > currentY) goalY--;
			if (goalY < currentY) goalY++;
			
			if ( distance(currentX, currentY, goalX, goalY) == 0 ) return 0;
			
            objId = livingLifePage->hetuwGetObjId( goalX, goalY );
            destUnknown = objId < 0 || objId > maxObjects;
		}
		
	}
	
	GridPos goal = {goalX, goalY};
	
	char* blockedMap = NULL;
	
	if (ghostMode) {
		int numPathMapCells = pathFindingD * pathFindingD;
		blockedMap = new char[ numPathMapCells ];
		memset( blockedMap, false, numPathMapCells );
		updateBlockedMap(blockedMap, true);
	} else {
		blockedMap = bMap;
	}
	
	
	int pathLength = 0;
	GridPos* path = NULL;
	GridPos closestFound = {currentX, currentY};
	bool pathFound = false;
	
	pathFound = LunarModPathFind( pathFindingD, pathFindingD,
								blockedMap, 
								currentPos, goal,
								&( pathLength ), 
								&( path ),
								&( closestFound )
								);
								
	if (!pathFound) {
		pathFound = LunarModPathFind( pathFindingD, pathFindingD,
									blockedMap, 
									currentPos, closestFound,
									&( pathLength ), 
									&( path ),
									&( closestFound )
									);
	}
    
    delete [] blockedMap;
	
	if( pathFound ) {
		
		if ( posEqual(currentPos, closestFound) ) {
			
			if ( posEqual(goal, dest) ) return -1;
			
			return 0;
		}
		
		currentMoveStatus->autoMovePath = path;
		currentMoveStatus->autoMovePathLength = pathLength;
		
		if (ghostMode) {
			
			int x = closestFound.x;
			int y = closestFound.y;
			
			sendMove(path, pathLength);
			
			ourLiveObject->xd = x; //BUG
			ourLiveObject->yd = y;
			
			ourLiveObject->currentPos.x = x;
			ourLiveObject->currentPos.y = y;
			
			ourLiveObject->inMotion = false;
			ourLiveObject->pathToDest = NULL;
			
			doublePair screenViewCenter = { 
				double(ourLiveObject->currentPos.x * CELL_D), 
				double(ourLiveObject->currentPos.y * CELL_D) };
			livingLifePage->lunarSetLastScreenViewCenter( screenViewCenter );
            
            delete [] blockedMap;
			
			return 0;
		}
		
		int targetX;
		int targetY;
		
		//Check for doors on the path
		int doorX, doorY;
		bool doorFoundInNextStep = false;
		
		for (int i = 0; i < pathLength; i++) {
			if ( i <= 2 ) {
				int objId = livingLifePage->hetuwGetObjId( path[i].x, path[i].y );
				ObjectRecord* obj = getObject( objId );
				if ( isClosedDoor(obj) ) {
					targetX = path[i-1].x;
					targetY = path[i-1].y;
					doorX = path[i].x;
					doorY = path[i].y;
					doorFoundInNextStep = true;
					break;
				}
			}
		}
		
		int pathIndexAsNextStep = 1;
		ObjectRecord* holdingObj = getObject(ourLiveObject->holdingID);
		if ( holdingObj != NULL ) {
			if ( holdingObj->speedMult > 2.5 ) {
				pathIndexAsNextStep = pathLength - 1;
			}
		}
		
		bool gonnaOpenDoor = false;
		if ( doorFoundInNextStep ) {
			
			if ( abs(doorX - currentX) + abs(doorY - currentY) == 1 ) {
			
				gonnaOpenDoor = true;
			
			} else if ( abs(doorX - targetX) == 1 && abs(doorY - targetY) == 1 ) {
				
				if ( tileIsSafeToWalk(doorX, targetY) ) {
					targetX = doorX;
				} else if ( tileIsSafeToWalk(targetX, doorY) ) {
					targetY = doorY;
				} else {
					printf("DEBUG AUTOMOVE: DOOR BLOCKED.\n");
					return -1;
				}
				
			} else {
				targetX = path[1].x;
				targetY = path[1].y;
			}
			
			if (gonnaOpenDoor) {
				openDoor(doorX, doorY);
				return 0;
			}
			
		} else {
			targetX = path[pathIndexAsNextStep].x;
			targetY = path[pathIndexAsNextStep].y;
		}

		targetX *= CELL_D;
		targetY *= CELL_D;
		livingLifePage->hetuwClickMove(targetX, targetY);
		
	} else {
		printf( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n" );
		printf( "PATH NOT FOUND\n" );
		printf( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n" );
		return -1;
	} 
	return 0;
}

//return 1 if done, 0 if not
int LunarMod::eat(GridPos pos) {
	if (ourLiveObject->currentSpeed != 0) return 0;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return 0;
	sendAction("SELF", pos.x, pos.y, -1);
	currentMoveStatus->waitingToUseOnSelf = true;
	return 1;
}

//return 1 if done, 0 if not
int LunarMod::use(GridPos pos) {
	if (ourLiveObject->currentSpeed != 0) return 0;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return 0;
	sendAction("USE", pos.x, pos.y);
	currentMoveStatus->waitingToGet = true;
	return 1;
}

//return 1 if done, 0 if not
int LunarMod::pick(GridPos pos) {
	if (ourLiveObject->currentSpeed != 0) return 0;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return 0;
	int destId = livingLifePage->hetuwGetObjId(pos.x, pos.y);
	int holdingId = ourLiveObject->holdingID;
	ObjectRecord* o = getObject(destId);
	TransRecord *tr = getTrans(0, destId);
	if (holdingId == 0 && tr != NULL && !o->permanent) {
		sendAction("REMV", pos.x, pos.y);
	} else {
		sendAction("USE", pos.x, pos.y);
	}
	currentMoveStatus->waitingToGet = true;
	return 1;
}

//return 1 if done, 0 if not
int LunarMod::drop(GridPos pos) {
	if (ourLiveObject->currentSpeed != 0) return 0;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return 0;
	sendAction("DROP", pos.x, pos.y);
	currentMoveStatus->waitingToDrop = true;
	return 1;
}

//return 1 if done, 0 if not
int LunarMod::yeet(GridPos pos) {
	if (ourLiveObject->currentSpeed != 0) return 0;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return 0;
	int id = ourLiveObject->holdingID;
	TransRecord *tr = getTrans(id, -1);
	ObjectRecord* o = getObject(id);
	if (tr != NULL && tr->newActor == 0 && o->permanent) {
		sendAction("USE", pos.x, pos.y);
	} else {
		sendAction("DROP", pos.x, pos.y);
	}
	currentMoveStatus->waitingToDrop = true;
	return 1;
}

//return 1 if arrived and acted, 0 if not, -1 when path not found
int LunarMod::moveAndPick(GridPos dest, bool force) {

	GridPos targetPos = getCloseEnoughTile(dest);
	if ( posEqual(targetPos, posNOTFOUND) ) return -1;
	
	int status = move(targetPos.x, targetPos.y);
	
	if (status != 0) {
		if (status == -1) {
			printf("DEBUG moveAndPick path not found.\n");
			return -1;
		}
		
		if ( isOnHorse(ourLiveObject) ) {
			if (currentX != ourLiveObject->currentPos.x || currentY != ourLiveObject->currentPos.y) return 0; //BUG
			currentMoveStatus->targetPos = targetPos;
			currentMoveStatus->droppingHorse = true;
			return 0;
		}
		
		if (currentMoveStatus->droppingHorse_result == -1) {
			currentMoveStatus->droppingHorse_result = 1;
			return -1;
		}
		
		int act_status = 0;
		if (force) act_status = pick(dest);
		if (!force) act_status = use(dest);
		if (act_status == 1) {
			// if (currentMoveStatus->wasOnHorse) currentMoveStatus->gettingBackOnHorse = true; //BUG
			return 1;
		}
	}

	return 0;
}

//return 1 if arrived and acted, 0 if not, -1 when path not found
int LunarMod::moveAndYeet(GridPos dest, bool force) {

	GridPos targetPos = getCloseEnoughTile(dest);	
	if ( posEqual(targetPos, posNOTFOUND) ) return -1;
	
	int status = move(targetPos.x, targetPos.y);
	
	if (status != 0) {
		if (status == -1) {
			printf("DEBUG moveAndYeet path not found.\n");
		}
		
		int act_status = 0;
		if (force) act_status = yeet(dest);
		if (!force) act_status = drop(dest);
		if (act_status == 1) {
			return 1;
		}
	}

	return 0;
}





void LunarMod::openDoor(int doorX, int doorY) {
	if (ourLiveObject->currentSpeed != 0) return;
	if (ourLiveObject->xd != ourLiveObject->xServer || ourLiveObject->yd != ourLiveObject->yServer) return;
	sendAction("USE", doorX, doorY);
	currentMoveStatus->waitForDoorToOpen = true;
	currentMoveStatus->lastDoorPos = {doorX, doorY};
}

//return 1 if done, return 0 if not, return -1 if no reachable tile to drop horse (or if no walk back allowed)
int LunarMod::dropHorse() {
	if ( ourLiveObject->currentSpeed != 0 ) return 0;
	if ( !isOnHorse(ourLiveObject) ) return 1;
	
	if (currentX != ourLiveObject->xServer || currentY != ourLiveObject->yServer) return 0; //BUG
	if (currentX != ourLiveObject->xd || currentY != ourLiveObject->yd) return 0; //BUG
	
	int dropped_id = ourLiveObject->holdingID;
	TransRecord *tr = getTrans(dropped_id, -1);
	if (tr != NULL) dropped_id = tr->newTarget;
	ObjectRecord* dropped_obj = getObject(dropped_id);
	bool dropped_blocking = true;
	if (dropped_obj != NULL) dropped_blocking = dropped_obj->blocksWalking;
	GridPos posDropHorse = getClosestTile(currentPos, 0, dropped_blocking);
	
	if ( posEqual(posDropHorse, posNOTFOUND) ) return -1;
	
	if ( !isAdjacent(posDropHorse, currentPos) && !posEqual(posDropHorse, currentPos) ) {
		if (!currentMoveStatus->droppingHorse_allowWalkBack) return -1;
		int moveDrop_status = move(posDropHorse.x, posDropHorse.y);
		if (moveDrop_status == 0) return 0;
		if (moveDrop_status == -1) return -1;
		// return 0;
		// return -1;
	}

	currentMoveStatus->rideObjId = ourLiveObject->holdingID;
	currentMoveStatus->wasOnHorse = true;
	
	TransRecord* t = getTrans( currentMoveStatus->rideObjId, -1 );
	if (t == NULL) sendAction("DROP", posDropHorse.x, posDropHorse.y, -1, -1, false);
	if (t != NULL) sendAction("USE", posDropHorse.x, posDropHorse.y, -1, -1, false);
	
	currentMoveStatus->waitingToDrop = true;
	currentMoveStatus->lastDropPos = posDropHorse;
	currentMoveStatus->horsePos = posDropHorse;	
	return 0;
	
}

//return 1 if done, return 0 if not, return -1 if no reachable tile to horse dropped pos
int LunarMod::getHorse() {
	GridPos targetPos = getCloseEnoughTile(currentMoveStatus->horsePos);	
	if ( posEqual(targetPos, posNOTFOUND) ) return -1;
	int move_status = move(targetPos.x, targetPos.y);
	if ( move_status != 1 ) return 0;
	int use_status = use(currentMoveStatus->horsePos);
	if (use_status == 1) currentMoveStatus->waitingToGet = true;
	return use_status;
}

int LunarMod::useBackpack() {
	if ( ourLiveObject->currentSpeed != 0 ) return 0;
	if ( ourLiveObject->inMotion ) return 0;
	if (currentX != ourLiveObject->currentPos.x || currentY != ourLiveObject->currentPos.y) return 0;
	if (currentX != ourLiveObject->xd || currentY != ourLiveObject->yd) return 0;
	// sendAction("SELF", currentX, currentY, 5); // backpack clothing slot
	sendAction("SELF", round(ourLiveObject->xd), round(ourLiveObject->yd), 5); // backpack clothing slot
	return 1;
}





void LunarMod::allModsOff() {
	
	moveReset();
	
	if (afkMode) {
		afkMode = false;
	}
	
	if (autoMode) {
		autoMode = false;
	}
	
	if (returnMode) {
		returnMode = false;
	}	
	
	if (searchMode) {
		searchMode = false;
	}
	
	if (killMode) {
		killMode = false;
	}
	
	if (followMode) {
		followMode = false;
	}
	
}

void LunarMod::autoMove() {
	
	int destX = currentX;
	int destY = currentY;
	
	if (currentMoveStatus->automoveDirection.find(strUpKey) != string::npos) destY += 9999;
	if (currentMoveStatus->automoveDirection.find(strLeftKey) != string::npos) destX -= 9999;
	if (currentMoveStatus->automoveDirection.find(strDownKey) != string::npos) destY -= 9999;
	if (currentMoveStatus->automoveDirection.find(strRightKey) != string::npos) destX += 9999;

	if (move(destX, destY) == 1) {
		moveReset();
		autoMode = false;
	}
}

void LunarMod::autoReturn() {
	if (move(savedX, savedY) == 1) {
		allModsOff();
	}
}

void LunarMod::autoFollow(int playerID, float dist) {
	
	LiveObject *player = livingLifePage->getLiveObject(playerID);
	if (player == NULL) return;
	
	int targetX = round(player->currentPos.x);
	int targetY = round(player->currentPos.y);
	
	if (dist != 0) {
		float dist_old = distance(currentX, currentY, targetX, targetY);
		if (dist_old < 1) dist_old = dist;
		
		float dx =  (currentX - targetX) * dist / dist_old;
		float dy =  (currentY - targetY) * dist / dist_old;

		targetX = round(targetX + dx);
		targetY = round(targetY + dy);
	}
	
	move(targetX, targetY);
	
}

void LunarMod::autoSearchHistoryReset() {
	delete autoSearchStatus;
	autoSearchStatus = new spiralSearchStatus();

	autoSearchStatus->searchBoundNegX = currentX - pathFindingD/2;
	autoSearchStatus->searchBoundPosX = currentX + pathFindingD/2;	
	autoSearchStatus->searchBoundNegY = currentY - pathFindingD/2;
	autoSearchStatus->searchBoundPosY = currentY + pathFindingD/2;
	
	updateSpiralSearchStatus(autoSearchStatus);

	foundPosHistory.clear();
}

void LunarMod::autoSearchContinue() {
	
	delete autoSearchStatus;
	autoSearchStatus = new spiralSearchStatus();
	
	int largerNum;
	int x = currentX;
	int y = currentY;
	if (abs(x) >= abs(y)) largerNum = abs(x);
	if (abs(x) < abs(y)) largerNum = abs(y);
	autoSearchStatus->searchBoundNegX = -largerNum;
	autoSearchStatus->searchBoundPosX = largerNum;
	autoSearchStatus->searchBoundNegY = -largerNum;
	autoSearchStatus->searchBoundPosY = largerNum;
	
	if (x>=y) {
		if (abs(x) >= abs(y)) {
			autoSearchStatus->nextSearchDirection = 0;
		} else {
			autoSearchStatus->nextSearchDirection = 3;
		}
	} else {
		if (abs(x) >= abs(y)) {
			autoSearchStatus->nextSearchDirection = 2;
		} else {
			autoSearchStatus->nextSearchDirection = 1;
		}		
	}

	foundPosHistory.clear();
}

//return 1 if search found, 0 if not, -1 when just arrived at reachPos
int LunarMod::autoSearch( bool* searchArray, vector<GridPos> &historyArray, bool active, bool verbose ) {
	
	currentMoveStatus->autoSearchReachPos = getClosestTile(currentPos, searchArray, historyArray);	
	
	if ( !posEqual(currentMoveStatus->autoSearchReachPos, posNOTFOUND) ) {
		
		int status = move(currentMoveStatus->autoSearchReachPos.x, currentMoveStatus->autoSearchReachPos.y);
		
		if (status == 1 || status == -1) {
			
			if (verbose && !posEqual(currentMoveStatus->autoSearchReachPos, currentMoveStatus->horsePos)) { //BUG
				char message[64];
				sprintf( message, "EMOT 0 0 %i#", 10);
				livingLifePage->sendToServerSocket( message );
				sprintf( message, "EMOT 0 0 %i#", 11);
				livingLifePage->sendToServerSocket( message );
				
				int c = char(historyArray.size() % 26 + 65);
				HetuwMod::addHomeLocation( currentMoveStatus->autoSearchReachPos.x, currentMoveStatus->autoSearchReachPos.y, HetuwMod::hpt_custom, c, -1 );
				
				int objId = livingLifePage->hetuwGetObjId( currentMoveStatus->autoSearchReachPos.x, currentMoveStatus->autoSearchReachPos.y );
				printf("LUNARMOD: SEARCH FOUND objId = %d, %d, %d\n", objId, currentMoveStatus->autoSearchReachPos.x, currentMoveStatus->autoSearchReachPos.y);
			}
			
			historyArray.push_back(currentMoveStatus->autoSearchReachPos);
			currentMoveStatus->autoSearchReachPos = posNOTFOUND;
			
			return -1;
		}
		return 1;
	}
	
	if ( active ) {
		GridPos searchPos = getSpiralSearchPos(autoSearchStatus);

		int status = move(searchPos.x, searchPos.y);

		if (status != 0) updateSpiralSearchStatus(autoSearchStatus);
		
	}
	
	return 0;
}

//return true if food found nearby, false if not
bool LunarMod::autoEat(bool wasteFood, bool active) {

	if ( currentMoveStatus->eatingPhaseNumber == 0 ) wasteFood = true;
	
	int holdingID = ourLiveObject->holdingID;
	ObjectRecord* holdingObj = getObject(holdingID);
	int foodValue = 0;
	if (holdingObj != NULL) foodValue = getObject(holdingID)->foodValue;
	
	bool canEatNow = false;
	if ( wasteFood ) canEatNow = ourLiveObject->foodStore < ourLiveObject->foodCapacity;
	if ( !wasteFood ) canEatNow = ourLiveObject->foodStore + foodValue + 2 <= ourLiveObject->foodCapacity;

	if( foodValue > 0 && canEatNow ) {
		currentMoveStatus->eatingPhaseOn = true;
		int eat_status = eat(currentPos);
		if (eat_status == 1 && ourLiveObject->foodStore + foodValue + 2 >= ourLiveObject->foodCapacity) {
			currentMoveStatus->eatingPhaseNumber++;
			currentMoveStatus->eatingPhaseOn = false;
			
			if (currentMoveStatus->wasOnHorse) {
				currentMoveStatus->gettingBackOnHorse = true;
				return true;
			}
		}
		return true;
	}
	
	GridPos foodPos = getClosestTile(currentPos, foodSearchArray, currentMoveStatus->foodFoundPosHistory);
	
	if ( foodValue == 0 ) {
		if ( !posEqual(foodPos, posNOTFOUND) ) {
			int foodSrcId = livingLifePage->hetuwGetObjId( foodPos.x, foodPos.y );
			TransRecord* pick_trans = getTrans( 0, foodSrcId );
			if (pick_trans != NULL) {
				int foodId = pick_trans->newActor;
				ObjectRecord* food = getObject(foodId);
				if (food != NULL) {
					foodValue = food->foodValue;
				}
			}
		}
	}
	
	int horseExtraBuffer = 5;
	if (foodValue == 0) foodValue = 5;

	bool needToReachFoodNow = false;
	if ( !isOnHorse(ourLiveObject) ) needToReachFoodNow = ourLiveObject->foodStore + foodValue + 2 <= ourLiveObject->foodCapacity;
	if ( isOnHorse(ourLiveObject) ) needToReachFoodNow = ourLiveObject->foodStore + foodValue + horseExtraBuffer + 2 <= ourLiveObject->foodCapacity;
	if ( currentMoveStatus->eatingPhaseNumber == 0 ) needToReachFoodNow = ourLiveObject->foodStore < ourLiveObject->foodCapacity;
	if ( currentMoveStatus->eatingPhaseOn ) needToReachFoodNow = true;
	
	bool needToDropHorse = ourLiveObject->foodStore + foodValue + 2 <= ourLiveObject->foodCapacity;
	if ( currentMoveStatus->eatingPhaseNumber == 0 ) needToDropHorse = ourLiveObject->foodStore < ourLiveObject->foodCapacity;
	if ( currentMoveStatus->eatingPhaseOn ) needToDropHorse = true;
	
	if ( posEqual(foodPos, posNOTFOUND) ) {
		
		if (currentMoveStatus->wasOnHorse) {
			currentMoveStatus->gettingBackOnHorse = true;
			return true; //food not found but step needs return //BUG
		}
		
		if (active) {
			if (currentMoveStatus->autoEatSearchStatus == NULL) {
				currentMoveStatus->autoEatSearchStatus = new spiralSearchStatus();
				updateSpiralSearchStatus(currentMoveStatus->autoEatSearchStatus);
			}
			GridPos foodSearchPos = getSpiralSearchPos(currentMoveStatus->autoEatSearchStatus);
			int move_status = move(foodSearchPos.x, foodSearchPos.y);
			if (move_status != 0) updateSpiralSearchStatus(currentMoveStatus->autoEatSearchStatus);
		}
		
	} else {
		
		if ( !posEqual(foodPos, currentMoveStatus->lastFoodPos) ) {
			if (currentMoveStatus->wasOnHorse) {
				currentMoveStatus->gettingBackOnHorse = true;
				return true;
			}
		}
		
		currentMoveStatus->lastFoodPos = foodPos;	
		
		if (!active && needToReachFoodNow) {
			int moveUse_status = moveAndPick(foodPos);
			if (moveUse_status == -1) {
				currentMoveStatus->foodFoundPosHistory.push_back(foodPos);
				return false;
			}
			return true;
		}
		
		if (active) {
			int status;
			if ((isOnHorse(ourLiveObject) && !needToDropHorse) || 
				//prevent USE spam when waiting to get hungry on ground
				(!isOnHorse(ourLiveObject) && ourLiveObject->holdingID != 0) ) { 
				status = move(foodPos.x, foodPos.y);
			} else {
				status = moveAndPick(foodPos);
			}
			if (status == -1) {
				currentMoveStatus->foodFoundPosHistory.push_back(foodPos);
				return false;
			}
			return true;
		}
		
	}
	
	return false;
}

//return 1 if arrived, 0 if not, -1 when arrived at closestFound
int LunarMod::ghostMove(int destX, int destY) {
	
	if (destX == currentX && destY == currentY) {
		return 1;
	}
	
	if (ourLiveObject->heldByAdultID != -1) return 0;
	
	
	int pathLength = 0;
	GridPos* path = NULL;
	GridPos closestFound = {currentX, currentY};
	bool pathFound = false;
	
	pathFound = LunarModPathFind( pathFindingD, pathFindingD,
								bMap, 
								currentPos, {destX, destY},
								&( pathLength ), 
								&( path ),
								&( closestFound )
								);
								
	if (!pathFound || path == NULL) {
		pathFound = LunarModPathFind( pathFindingD, pathFindingD,
									bMap, 
									currentPos, closestFound,
									&( pathLength ), 
									&( path ),
									&( closestFound )
									);
	}

	
	if (path != NULL && pathLength != 0) {
		
		int x = closestFound.x;
		int y = closestFound.y;
		
		sendMove(path, pathLength);
		
		// ourLiveObject->xd = x;
		// ourLiveObject->yd = y;
		
		ourLiveObject->currentPos.x = x;
		ourLiveObject->currentPos.y = y;
		
		ourLiveObject->inMotion = false;
		ourLiveObject->pathToDest = NULL;
		
		doublePair screenViewCenter = { 
			double(ourLiveObject->currentPos.x * CELL_D), 
			double(ourLiveObject->currentPos.y * CELL_D) };
		livingLifePage->lunarSetLastScreenViewCenter( screenViewCenter );
		
		return 1;
	}
	
	return 0;
}

bool LunarMod::ghostModeAwake() {
	
	int x = ourLiveObject->xServer;
	int y = ourLiveObject->yServer;
	
	std::uniform_int_distribution<int> uni3(0, 2);
	int xDir = uni3(rng);
	int yDir = uni3(rng);
	
	int xInc = 0;
	int yInc = 0;
	if (xDir == 0) xInc = -1;
	if (xDir == 2) xInc = 1;
	if (yDir == 0) yInc = -1;
	if (yDir == 2) yInc = 1;
	
	int dist = 28;
	
	for ( int i=0; i<dist; i++ ) {
		sendStep(x, y, xInc, yInc);
		x = x + xInc;
		y = y + yInc;
	}
	
	for ( int i=0; i<dist; i++ ) {
		sendStep(x, y, -xInc, -yInc);
		x = x - xInc;
		y = y - yInc;
	}

	
	x = ourLiveObject->xServer;
	y = ourLiveObject->yServer;
	ourLiveObject->currentPos.x = x;
	ourLiveObject->currentPos.y = y;
	ourLiveObject->xd = x;
	ourLiveObject->yd = y;
	ourLiveObject->inMotion = false;
	ourLiveObject->pathToDest = NULL;
	
	doublePair screenViewCenter = { double(ourLiveObject->currentPos.x * CELL_D), double(ourLiveObject->currentPos.y * CELL_D) };
	livingLifePage->lunarSetLastScreenViewCenter( screenViewCenter );


	
	int unknownCheckRadius = 8;
	int unknownTileCount = 0;
	for (int i=-unknownCheckRadius; i<=unknownCheckRadius; i++) {
		for (int j=-unknownCheckRadius; j<=unknownCheckRadius; j++) {
			int mI = livingLifePage->hetuwGetMapI( currentX + i, currentY + j );
			if ( mI >= 0 && livingLifePage->mMapBiomes[mI] < 0 ) unknownTileCount++;
		}
	}

	int mapI = livingLifePage->hetuwGetMapI( ourLiveObject->xServer, ourLiveObject->yServer );
	int posBiome = -1;
	if ( mapI >= 0 ) {
		posBiome = livingLifePage->mMapBiomes[mapI];
	}
	
	if (posBiome >= 0 && unknownTileCount == 0) return true;
	
	return false;
}

bool LunarMod::autoKill(int playerID) {
	
	LiveObject *me = ourLiveObject;
	LiveObject *victim = livingLifePage->getLiveObject(playerID);
	if (victim == NULL) return true;
	
	GridPos victimPos = { round(victim->currentPos.x), round(victim->currentPos.y) };
	GridPos ourPos = { round(ourLiveObject->currentPos.x), round(ourLiveObject->currentPos.y) };
	
	float dist = distance(victimPos, ourPos);
	
	int killModeSafeDist;
	int killModeDist;
	int fleeDist;
	if ( killModeIndex == 0 ) {
		//antigrief mode
		killModeSafeDist = 1;
		killModeDist = 8;
		fleeDist = 12;
	} else if ( killModeIndex == 1 ) {
		//pvp mode
		killModeSafeDist = 6;
		killModeDist = 8;
		fleeDist = 12;
	} else if ( killModeIndex == 2 ) {
		//killaura mode
		if ( dist <= 8 && !isKilling(ourLiveObject) ) {
			char *killMessage = 
				autoSprintf( "KILL 0 0 %d#",
							 playerID );
			livingLifePage->sendToServerSocket( killMessage );
		}
		return false;
	}
	
	bool weaponOnHand_me = false;
	int deadlyDist = 0;
	ObjectRecord *weapon = getObject( me->holdingID );
	if ( weapon != NULL ) {
		if ( weapon->deadlyDistance > 0 ) {
			weaponOnHand_me = true;
			deadlyDist = weapon->deadlyDistance;
		}
	}
	
	ObjectRecord *backpack_me = (&(me->clothing))->backpack;
	bool backpackHasWeapon_me = false;
	bool backpackCanHoldWeapon_me = false;
	if ( backpack_me != NULL ) {
		// backpackHasWeapon_me = backpack_me->id == 3050 || backpack_me->id == 6990;
		// backpackCanHoldWeapon_me = backpack_me->id == 198 || backpack_me->id == 6633;
		
		backpackCanHoldWeapon_me = false;
		
		TransRecord* getWeapon_trans = getTrans( 0, backpack_me->id );
		if ( getWeapon_trans != NULL ) {
			ObjectRecord *weapon = getObject( getWeapon_trans->newActor );
			if ( weapon != NULL ) {
				if ( weapon->deadlyDistance > 0 ) backpackHasWeapon_me = true;
			}
		}
	}
	
	ObjectRecord *backpack_victim = (&(victim->clothing))->backpack;
	int id_backpack_victim = 0;
	if (backpack_victim != NULL) {
		id_backpack_victim = backpack_victim->id;
	}
	bool victimIsDangerous = true;
	victimIsDangerous = isWeapon( victim->holdingID ) || 
						(!isOnHorse(victim) && (id_backpack_victim == 3050 || id_backpack_victim == 6990) );
	if ( !victimIsDangerous ) {
		killModeDist = killModeSafeDist;
	}
	
	
	if ( !isKilling(victim) ) {
		victimKillFirst = false;
	}
	if ( isKilling(victim) && !isKilling(me) ) { //BUG
		victimKillFirst = true;
	}
	
	if ( victimKillFirst ) {
		if (currentMoveStatus->wasOnHorse) {
			if (me->holdingID != 0) {
				if ( backpackCanHoldWeapon_me ) {
					int useBackPackStatus = useBackpack();
					if (useBackPackStatus == 1) currentMoveStatus->waitingToDrop = true;
					return false;
				} else {					
					int dropWeaponStatus = 0;
					dropWeaponStatus = moveAndYeet(currentMoveStatus->horsePos); //BUG
					// if (dropWeaponStatus == 1) currentMoveStatus->waitingToDrop = true;
					return false;
				}
			}
			currentMoveStatus->gettingBackOnHorse = true;
			return false;
		} else {
			autoFollow(playerID, (float)fleeDist);
			return false;
		}
	}
	
	if ( dist > killModeDist && currentMoveStatus->wasOnHorse ) {
		if (me->holdingID != 0) {
			if ( backpackCanHoldWeapon_me ) {
				int useBackPackStatus = useBackpack();
				if (useBackPackStatus == 1) currentMoveStatus->waitingToDrop = true;
				return false;
			} else {					
				int dropWeaponStatus = 0;
				dropWeaponStatus = moveAndPick(currentMoveStatus->horsePos, false); //BUG
				// if (dropWeaponStatus == 1) currentMoveStatus->waitingToDrop = true;
				return false;
			}
		}
		currentMoveStatus->gettingBackOnHorse = true;
		return false;
	}
	
	if ( dist > killModeDist ) {
		autoFollow(playerID, 0);
		return false;
	}
	
	if ( dist <= killModeDist && !isKilling(me) && weaponOnHand_me ) {
		char *killMessage = 
			autoSprintf( "KILL 0 0 %d#",
						 playerID );
		livingLifePage->sendToServerSocket( killMessage );
		return false;
	}
	
	if ( dist <= killModeDist && !isKilling(me) && isOnHorse(me) ) {
		
		if (currentX != ourLiveObject->currentPos.x || currentY != ourLiveObject->currentPos.y) return 0; //BUG
		if (currentX != ourLiveObject->xServer || currentY != ourLiveObject->yServer) return 0; //BUG
		if (currentX != ourLiveObject->xd || currentY != ourLiveObject->yd) return 0; //BUG
		if ( ourLiveObject->currentSpeed != 0 ) return 0;
		
		GridPos currentPos = { currentX, currentY };
		GridPos emptyPos = getClosestTile(currentPos, 0);
		
		if ( distance( currentPos, emptyPos ) <= 1 && distance(victimPos, emptyPos) > 0 ) {
			currentMoveStatus->targetPos = emptyPos;
			currentMoveStatus->droppingHorse = true;
			return false;
		} else {
			autoFollow(playerID, 0);
			return false;
		}
	}
	
	if ( dist <= killModeDist && !isKilling(me) && !weaponOnHand_me && backpackHasWeapon_me ) {
		int useBackPackStatus = useBackpack();
		if (useBackPackStatus == 1) currentMoveStatus->waitingToGet = true;
		return false;
	}
	
	if ( dist <= killModeDist && !isKilling(me) && !weaponOnHand_me && currentMoveStatus->wasOnHorse ) {
		bool horseHoldsWeapon = false;
		int mapI = livingLifePage->hetuwGetMapI( currentMoveStatus->horsePos.x, currentMoveStatus->horsePos.y );
		if (mapI > 0) {
			if (HetuwMod::mMapContainedStacks[mapI].size() > 0) {
				for (int i=0; i < HetuwMod::mMapContainedStacks[mapI].size(); i++) {
					int objId = *HetuwMod::mMapContainedStacks[mapI].getElement(i);
					if ( isWeapon(objId) ) {
						horseHoldsWeapon = true;
						break;
					}
				}
			}
		}
		if ( horseHoldsWeapon && !me->inMotion ) {
			currentMoveStatus->waitingToGet = true;
			sendAction("REMV", currentMoveStatus->horsePos.x, currentMoveStatus->horsePos.y);
			return false;	
		}
	}
	
	if ( dist <= killModeDist && !isKilling(me) && !weaponOnHand_me && !isOnHorse(me) && !currentMoveStatus->wasOnHorse ) {
		//no weapon
		return false;
	}
	
	if ( dist <= killModeDist && isKilling(me) && weaponOnHand_me && !victimKillFirst ) {
		autoFollow(playerID, 0);
		return false;
	}
	
}






#include <chrono>

void LunarMod::loading() {
	
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;
    auto t0 = Time::now();
	
	// initTechMap();
	// initUndoMap();
	
    auto t1 = Time::now();
    fsec fs = t1 - t0;
    // ms d = std::chrono::duration_cast<ms>(fs);
    std::cout << "DEBUG: INIT_TIME = " << fs.count() << "s\n";
    // std::cout << d.count() << "ms\n";
	
}













string LunarMod::getObjectName(int id) {
	if (id == -2) return "-2";
	if (id == -1) return "-1";
	if (id == 0) return "0";
    ObjectRecord *o = getObject( id );
    if( o == NULL ) {
		return "";
	}
    string upper( stringToUpperCase( o->description ) );
    return upper;
}

bool LunarMod::isProbabilitySet(int objId) {
	if (objId <= 0) return false;
	CategoryRecord *c = getCategory( objId );
	if (c == NULL) return false;
    if( c->isProbabilitySet ) return true;
    return false;
}

bool LunarMod::transEqual(TransRecord* a, TransRecord* b) {
	if (a == NULL || b == NULL) return false;
	return
		a->actor == b->actor &&
		a->target == b->target &&
		a->newActor == b->newActor &&
		a->newTarget == b->newTarget;
}

void LunarMod::printTrans(TransRecord* t, bool printName, bool eof) {
	if ( t == NULL ) {
		printf("DEBUG: NULL");
		if (eof) printf("\n");
		return;
	}
	
	int idA = t->actor;
	int idB = t->target;
	int idC = t->newActor;
	int idD = t->newTarget;
	
	printf("DEBUG: %d + %d = %d + %d", idA, idB, idC, idD);
	
	if (printName) {
		string sA = getObjectName(idA);
		string sB = getObjectName(idB);
		string sC = getObjectName(idC);
		string sD = getObjectName(idD);
		
		printf(", %s + %s = %s + %s", sA.c_str(), sB.c_str(), sC.c_str(), sD.c_str());
	}
	
	if (eof) printf("\n");
	
}

int LunarMod::getUseDummyParent(int objId) {
	if (objId <= 0) return objId;
	ObjectRecord* o = getObject(objId);
	if (o == NULL) return objId;
	if (!o->isUseDummy) return objId;
	return o->useDummyParent;
}

bool LunarMod::transEqualIgnoringUse(TransRecord* a, TransRecord* b) {
	if (a == NULL || b == NULL) return false;
	return
		getUseDummyParent(a->actor) == getUseDummyParent(b->actor) &&
		getUseDummyParent(a->target) == getUseDummyParent(b->target) &&
		getUseDummyParent(a->newActor) == getUseDummyParent(b->newActor) &&
		getUseDummyParent(a->newTarget) == getUseDummyParent(b->newTarget);
}













vector<TransRecord*> LunarMod::getUsesTrans( int objId ) {
	
	SimpleVector<TransRecord*> *uses = getAllUses( objId );
	
	vector<TransRecord*> results;
	
	if( uses != NULL ) {
		for( int i=0; i<uses->size(); i++ ) {
			
			vector<TransRecord*> transQueue;
			
			TransRecord *origTrans = uses->getElementDirect( i );
			int cOrD = -1;
			if ( isProbabilitySet(origTrans->newActor) ) cOrD = 0;
			if ( isProbabilitySet(origTrans->newTarget) ) cOrD = 1;
			int origIdC = origTrans->newActor;
			int origIdD = origTrans->newTarget;
			if (cOrD != -1) {
				CategoryRecord* c;
				if (cOrD == 0) c = getCategory( origIdC );
				if (cOrD == 1) c = getCategory( origIdD );
				SimpleVector<int> idSet = c->objectIDSet;
				// SimpleVector<float> wSet = c->objectWeights;
				for (int i=0; i<idSet.size(); i++) {
					TransRecord* staticTrans = new TransRecord;
					*staticTrans = *origTrans;
					int newId = idSet.getElementDirect(i);
					if (cOrD == 0) staticTrans->newActor = newId;
					if (cOrD == 1) staticTrans->newTarget = newId;
					
					transQueue.push_back(staticTrans);
				}
			} else {
				transQueue.push_back(origTrans);
			}

			for ( int j=0; j<transQueue.size(); j++ ) {
				results.push_back( transQueue[j] );
			}
		}
	}
	
	return results;
}

vector<TransRecord*> LunarMod::getProdTrans( int objId ) {
	
	vector<TransRecord*> results;
	
	SimpleVector<TransRecord*> *prods = getAllProduces( objId );
	if( prods != NULL ) {
		for( int i=0; i<prods->size(); i++ ) {
			TransRecord *trans = prods->getElementDirect( i );
			results.push_back( trans );
		}
	}
	
	int numCategoriesForObject = getNumCategoriesForObject( objId );
	for (int i=0; i<numCategoriesForObject; i++) {
		int cId = getCategoryForObject(objId, i);
		CategoryRecord* c = getCategory( cId );
		
		if (c->isProbabilitySet) {
			SimpleVector<TransRecord*> *prodPTrans = getAllProduces( cId );
			int numPTrans = 0;
			if( prodPTrans != NULL ) {
				numPTrans = prodPTrans->size();
			}
			for( int t=0; t<numPTrans; t++ ) {
				TransRecord* staticTrans = new TransRecord;
				*staticTrans = *(prodPTrans->getElementDirect( t ));
				
				if (staticTrans->actor == cId) staticTrans->actor = objId;
				if (staticTrans->target == cId) staticTrans->target = objId;
				if (staticTrans->newActor == cId) staticTrans->newActor = objId;
				if (staticTrans->newTarget == cId) staticTrans->newTarget = objId;
				
				results.push_back(staticTrans);
			}
		}
	}
	
	return results;
}

int LunarMod::getTransIndex( vector<TransRecord*> transVector, TransRecord* trans ) {
	if ( trans == NULL ) return -1;
	for (int i = 0; i<transVector.size(); i++) {
		if ( transEqual(transVector[i], trans) ) return i;
	}
	// int i = 0;
	// for (auto& it : transVector) {
		// if ( transEqual(it, trans) ) return i;
		// i++;
	// }
	return -1;
}








void LunarMod::printTree( vector<TransRecord*> tree ) {
	for (int i=0; i<tree.size(); i++) {
		TransRecord* t = tree[i];
		printTrans(t);
	}
}

void LunarMod::initTechMap() {
	
	sizeMap.resize(maxObjects, UNREACHABLE);
	depthMap.resize(maxObjects, UNREACHABLE);
	techMap.resize(maxObjects, NULL);
	
	vector<int> treeHorizon;
	
    for( int i=1; i<maxObjects; i++ ) {
		if ( getObjectDepth(i) == 0 ) {
			sizeMap[i] = 0;
			depthMap[i] = 0;
			techMap[i] = NULL;
			treeHorizon.push_back(i);
		}
	}
	
    int index = 0;
    while( index < treeHorizon.size() ) {
		
		int nextID = treeHorizon[index];
		
		vector<TransRecord*> uses = getUsesTrans( nextID );
		
		for( int i=0; i<uses.size(); i++ ) {
			TransRecord *tr = uses[i];
			
			if ( tr != NULL && getTransIndex(allTrans, tr) == -1 ) {
				allTrans.push_back(tr); 
			}
			
			int transSize = UNREACHABLE;
			int transDepth = UNREACHABLE;
			
			if( tr->target <= 0 ) {
				transSize = sizeMap[tr->actor] + 1;
				transDepth = depthMap[tr->actor] + 1;
			} else if ( tr->actor <= 0 ) {
				transSize = sizeMap[tr->target] + 1;
				transDepth = depthMap[tr->target] + 1;
			} else if( tr->actor > 0 && tr->target > 0 ) {
				transSize = sizeMap[tr->actor] + sizeMap[tr->target] + 1;
				transDepth = max(depthMap[tr->actor], depthMap[tr->target]) + 1;
			}
			
			if( transSize < UNREACHABLE ) {
				if( tr->newActor > 0 ) {
					if ( transSize < sizeMap[tr->newActor] ) {
					// if ( transDepth < depthMap[tr->newActor] ) { //depth/size change
						sizeMap[tr->newActor] = transSize;
						depthMap[tr->newActor] = transDepth;
						techMap[tr->newActor] = tr;
						treeHorizon.push_back( tr->newActor );
					}
				}
				if( tr->newTarget > 0 ) {
					if ( transSize < sizeMap[tr->newTarget] ) {
					// if ( transDepth < depthMap[tr->newTarget] ) { //depth/size change
						sizeMap[tr->newTarget] = transSize;
						depthMap[tr->newTarget] = transDepth;
						techMap[tr->newTarget] = tr;
						treeHorizon.push_back( tr->newTarget );
					}
				}
			}
			
		}
		
		index++;
	}
}

void LunarMod::initUndoMap() {
	if (false) {
		for (int i=0; i<allTrans.size(); i++) {
			undoMap.push_back( computeUndoCostMap(allTrans[i]) ); 
			printf("DEBUG: %d\n", undoMap.size());
		}
		
		std::string path("./undoMap.txt");
		std::ofstream ofs(path, std::ios::out | std::ofstream::binary);
		std::ostream_iterator<int> osi{ofs," "};
		std::copy(undoMap.begin(), undoMap.end(), osi);
	} else {
		std::string path("./undoMap.txt");
		std::ifstream ifs(path, std::ios::in | std::ifstream::binary);
		std::istream_iterator<int> iter{ifs};
		std::istream_iterator<int> end{};
		std::copy(iter, end, std::back_inserter(undoMap));
	}
}

int LunarMod::costFromTransMap( vector<TransRecord*> transMap, int objId ) {
	
	if (objId <= 0) return 0;
	TransRecord* decidedTrans = transMap[objId];
	if ( decidedTrans == NULL ) return sizeMap[objId];
	
	int cost = 0;
	
	if (decidedTrans->actor > 0) {
		cost += costFromTransMap(transMap, decidedTrans->actor);
	}
	if (decidedTrans->target > 0) {
		cost += costFromTransMap(transMap, decidedTrans->target);
	}
	
	cost += 1;
	
	return cost;
}

int LunarMod::computeUndoCostMap( TransRecord *trans ) {
	
	if (trans == NULL) return 0;
	
	vector<TransRecord*> transMap(maxObjects, NULL);
	vector<int> costMap(maxObjects, UNREACHABLE);
	vector<int> treeHorizon;
	
    for( int i=1; i<maxObjects; i++ ) {
		if ( i == trans->newActor || i == trans->newTarget ) {
			costMap[i] = 0;
			treeHorizon.push_back(i);
		}
	}
	
    int index = 0;
    while( index < treeHorizon.size() ) {
		
		int nextID = treeHorizon[index];
		
		vector<TransRecord*> transVectors = getUsesTrans( nextID );
		
		for( int i=0; i<transVectors.size(); i++ ) {
			TransRecord *t = transVectors[i];
			
			int cost = UNREACHABLE;
			
			if( t->target <= 0 ) {
				cost = costMap[t->actor] + 1;
			} else if ( t->actor <= 0 ) {
				cost = costMap[t->target] + 1;
			} else if( t->actor > 0 && t->target > 0 ) {
				cost = costMap[t->actor] + costMap[t->target] + 1; //size
				// cost = max(costMap[t->actor], costMap[t->target]) + 1; //depth
			}
			
			if( cost < UNREACHABLE ) {
				if( t->newActor > 0 ) {
					if ( cost < costMap[t->newActor] ) {
						costMap[t->newActor] = cost;
						
						transMap[t->newActor] = t;
						
						treeHorizon.push_back( t->newActor );
					}
				}
				if( t->newTarget > 0 ) {
					if ( cost < costMap[t->newTarget] ) {
						costMap[t->newTarget] = cost;
						
						transMap[t->newTarget] = t;
						
						treeHorizon.push_back( t->newTarget );
					}
				}
			}
		}
		
		index++;
	}
	
	return costFromTransMap(transMap, trans->actor) + costFromTransMap(transMap, trans->target);
}

vector<TransRecord*> LunarMod::computeTransMap( int objId, vector<bool> givenObjs ) {
	
	vector<TransRecord*> transMap(maxObjects, NULL);
	vector<int> costMap(maxObjects, UNREACHABLE);
	vector<int> treeHorizon;
	
    for( int i=1; i<maxObjects; i++ ) {
		if ( ( givenObjs[i] ) && i != objId ) {
		// if ( ( givenObjs[i] || getObjectDepth(i) == 0 ) && i != objId ) {
			costMap[i] = 0;
			treeHorizon.push_back(i);
		}
	}
	
    int index = 0;
    while( index < treeHorizon.size() ) {
		
		int nextID = treeHorizon[index];
		
		vector<TransRecord*> transVectors = getUsesTrans( nextID );
		
		for( int i=0; i<transVectors.size(); i++ ) {
			TransRecord *t = transVectors[i];
			
			int cost = UNREACHABLE;
			
			if( t->target <= 0 ) {
				cost = costMap[t->actor] + 1;
			} else if ( t->actor <= 0 ) {
				cost = costMap[t->target] + 1;
			} else if( t->actor > 0 && t->target > 0 ) {
				cost = costMap[t->actor] + costMap[t->target] + 1; //size
				// cost = max(costMap[t->actor], costMap[t->target]) + 1; //depth
			}
			
			if( cost < UNREACHABLE ) {
				
				bool calculateUndoCost = false;
				if( t->newActor > 0 ) {
					if (!transEqualIgnoringUse(techMap[t->newActor], t)) calculateUndoCost = true;
				}
				if( t->newTarget > 0 ) {
					if (!transEqualIgnoringUse(techMap[t->newTarget], t)) calculateUndoCost = true;
				}
				if (calculateUndoCost) {
					int undoCost = 0;
					
					// int transIndex = getTransIndex(allTrans, t);
					// if (transIndex != -1) undoCost = undoMap[transIndex];
					
					cost += undoCost;
				}
				
				
				if (true) {
					int timeCostMultiplier = 1;
					int timeCost = 0;
					
					// if (t->autoDecaySeconds > 0) timeCost = t->autoDecaySeconds * timeCostMultiplier;
					
					cost += timeCost;
				}
				
				
			}
			
			if( cost < UNREACHABLE ) {
				if( t->newActor > 0 ) {
					if ( cost < costMap[t->newActor] ) {
						costMap[t->newActor] = cost;
						
						transMap[t->newActor] = t;
						
						if ( t->newActor != objId ) {
							treeHorizon.push_back( t->newActor );
						}
					}
				}
				if( t->newTarget > 0 ) {
					if ( cost < costMap[t->newTarget] ) {
						costMap[t->newTarget] = cost;
						
						transMap[t->newTarget] = t;
						
						if ( t->newTarget != objId ) {
							treeHorizon.push_back( t->newTarget );
						}
					}
				}
			}
		}
		
		index++;
	}
	
	return transMap;
}

vector<TransRecord*> LunarMod::treeFromTransMap( vector<TransRecord*> transMap, int objId ) {
	
	
	vector<TransRecord*> tree;
	vector<int> targets;
	vector<int> queue;
	queue.push_back( objId );
	int i = 0;
	while ( i < queue.size() ) {
		TransRecord* t = transMap[ queue[i] ];
		if (t != NULL) {
			tree.push_back(t);
			targets.push_back( queue[i] );
			
			if (t->actor > 0) queue.push_back( t->actor );
			if (t->target > 0) queue.push_back( t->target );
		}
		i++;
	}
	
	reverse(tree.begin(), tree.end());
	
	for (int i=tree.size()-1; i>=0; i--) {
		
		TransRecord* t = tree[i];
		
		bool actor_seen = false;
		bool target_seen = false;
		for (int j=0; j<i; j++) {
			if (t->actor == targets[j]) actor_seen = true;
			if (t->target == targets[j]) target_seen = true;
		}
	
		if ( actor_seen && getUseDummyParent(t->actor) == getUseDummyParent(t->newActor) ) {
			tree.erase(tree.begin() + i, tree.end());
		}
	
	
	
	
	}
	
	
	
	
	return tree;
	
	
	
	// vector<TransRecord*> tree;
	// vector<int> queue;
	// queue.push_back( objId );
	// int i = 0;
	// while ( i < queue.size() ) {
		// TransRecord* t = transMap[ queue[i] ];
		// if (t != NULL) {
			// tree.push_back(t);
			// if (t->actor > 0) queue.push_back( t->actor );
			// if (t->target > 0) queue.push_back( t->target );
		// }
		// i++;
	// }
	// return tree;
	
	// vector<TransRecord*> tree;
	// TransRecord* t = transMap[ objId ];
	// if ( t == NULL ) return tree;
	// tree.push_back( t );
	// vector<TransRecord*> leftTree;
	// vector<TransRecord*> rightTree;
	// if ( t->actor > 0 ) leftTree = treeFromTransMap( transMap, t->actor );
	// if ( t->target > 0 ) rightTree = treeFromTransMap( transMap, t->target );
	
	// tree.insert(tree.end(), leftTree.begin(), leftTree.end());
	// tree.insert(tree.end(), rightTree.begin(), rightTree.end());
	// return tree;
}













int LunarMod::moveObj(GridPos fromPos, GridPos toPos, int objId) {
	
	bool posHasItem = livingLifePage->hetuwGetObjId(fromPos.x, fromPos.y) == objId;
	bool posIsEmpty = livingLifePage->hetuwGetObjId(fromPos.x, fromPos.y) == 0;
	bool handHasItem = ourLiveObject->holdingID == objId;
	bool handIsEmpty = ourLiveObject->holdingID != objId;
	
	if (posHasItem && handIsEmpty) {
		int stat = moveAndPick( fromPos );
		if (stat == -1) return -1;
		return 0;
	} else if (posIsEmpty && handHasItem) {
		int stat = moveAndYeet( getClosestTile( toPos, 0 ) );
		return stat;
	}
	
	return -1;
	
}



	













