#ifndef __IDENTIFIERS_H__
#define __IDENTIFIERS_H__

#include "types.h"

enum RenderLayer
{
	RL_Default = 0,
	RL_Enemy = 1,
	RL_Player = 2,
	RL_NumRenderLayers,
};

enum Facing
{
	Left = -1,
	Right = 1
};

enum class ColliderType
{
	NoColl,
	BoxColl,
	CircleColl
};

enum StatType
{
	Health = 1,
	Stamina = 3,
	Mana = 5,
};

enum class UI_ID
{
	Resolution,
	FullScreen,
	TextOnly,
	Apply,
	Quit,
	ResumeGame,
	VSync,
	ReturnToMap
};

enum class UI_Type
{
	Default,
	ArrowToggles,
	Tick_box,
	Button
};

namespace sf
{
	class Texture;
	class Font;
	class SoundBuffer;
}

// ------- IDENTIFIERS ---------------
namespace ID
{
	enum class Font
	{
		GOTHIC,
		ARIEL
	};

	enum class States
	{
		None = 0,
		Title = 1,
		Game = 2,
		Pause = 4,
		WorldMap = 8,
		Inventory = 16,
		GameOver = 32,
		Lobby = 64,
		Leaderboards = 128
	};

	enum class Texture
	{
		// Debug -----------------
		RAW_PIXEL_IMAGE = -1,		// This is one white pixel, you need to use a scaling matrix for exact debug drawing
		ERROR_IMAGE = -2,			// This is used as a fall back when the reqested image does not exist
		None = -3,
		//------------------------

		// Tile Sets -------------
		BloodMtn_TileSet = 9999,
		White_TileSet = 0,
		//------------------------

		// Backgrounds -----------
		Bkgrnd_BlackTiles,
		Bkgrnd_GreenGraveyard,
		Bkgrnd_PurpleCave,
		Bkgrnd_RedMtn,
		Bkgrnd_Pyramid,
		//------------------------

		// Game Objects ----------
		Player = 6,
		LevelExit,
		Spikes,
		SkullCurrency,
		Fireball,
		Slime,
		Boulder,

		// NOTE** ALL ENEMIES SHOULD BE GROUPED TOGETHER FOR FAST DYNAMIC CHECKING (or update quadtree, but will be more expensive)
		BlueMinionEnemy,
		MageEnemy,
		DiscipleEnemy,
		TaurusEnemy,
		ShadowEnemy,
		GreemHeadEnemy,
		ProjectileEnemy,
		DestructableWall,
		DissappearPlatform,
		CheckpointTorch,
		Potions,
		GoldSkull,

		// Art
		LevelSelectScreenArt,
		TitleScreenArt,
		PauseScreenArt,
		SkullBg,
		WorldMapArt,
		//------------------------

		NumTextures
	};

	enum class Audio
	{
		None = 0,
		M1 = -1,
		M2 = -2,
		M3 = -3,
		M4 = -4,
		M5 = -5,
		M6 = -6,
		GoldSkullCollect = 1,
		ProjectileHit,
		EnemyDead,
		UI_Select,
		JumpSound,
		EnemyProjectileFire,
		ProjectileFire,
		BossProjectilFire,
		ErrorSound,
		SelectSound,
		WallHit,
		FoundSecret,
		FootStep,
		PlayerHit,
		PauseSound,
		UnPauseSound,
		FlyingSound, // god knows
		PlayerDead,
		NumAudio
	};

	enum class Component
	{
		Transform,
		Renderer,
		Animation,
		BoxCollider,

		PlayerScript,
		ProjectileScript,
		HazardGeomotryScript,

		DestroyableScript,
		BlueMinionEnemyScript,
		MageEnemyScript,
		TaurusEnemyScript,
		EnemyProjectile,
		ShadowEnemyScript,
		DissaperaingPlatform,
		DropHazard,
		BouncingBoulderScript,
		CircleCollider,
		BossTrigger,
		ExitScript,
		CheckpointScript,
		GoldSkullScript,
		GreenHeadEnemy,
		NormalSkullScript,
		DiscipleEnemyScript,

		NumComponentTypes
	};

	enum Type
	{
		Empty,
		Wall,
		Player,
		Exit,
		Slime,
		Skull_Collect,
		Walking_Enemy,
		Health,
		Platform,
		Spike = 9,
		SpikeD = 10,
		SpikeL = 11,
		SpikeR = 12,
		EnemyBlueMinion = 13,
		EnemyMage,
		EnemyDisciple,
		EnemyTaurus,
		EnemyShadow,
		EnemyGreenHead,
		DissapearingPlatform,
		DestructablePlatform,
		DropHazard,
		BouncingBoulder,
		BossTrigger,
		CheckPoint,
		GoldSkull,

		NetworkType,

		//=================
		EnemyProjectile,
		PlayerProjectile,
		NUM_TYPES
	};


	// BIT MASK CONSTANTS
	const static uint64     Empty_ = 0;
	const static uint64		Wall_ = 1;
	const static uint64		Player_ = 2;
	const static uint64		Exit_ = 4;
	const static uint64		Slime_ = 8;
	const static uint64		Skull_Collect_ = 16;
	const static uint64		Walking_Enemy_ = 32;
	const static uint64		Health_ = 64;
	const static uint64		Platform_ = 128;
	const static uint64		Spike_ = 256;

	const static uint64		EnemyBlueMinion_ = 512;
	const static uint64		EnemyMage_ = 1024;
	const static uint64		EnemyDisciple_ = 2048;
	const static uint64		EnemyTaurus_ = 4096;
	const static uint64		EnemyShadow_ = 8192;
	const static uint64		EnemyGreenHead_ = 16384;
	const static uint64		DissapearingPlatform_ = 32768;
	const static uint64		DestructablePlatform_ = 65536;
	const static uint64		DropHazard_ = 131072;
	const static uint64		BouncingBoulder_ = 262144;
	const static uint64		BossTrigger_ = 524288;
	const static uint64		CheckPoint_ = 1048576;
	const static uint64		GoldSkull_ = 2097152;
	const static uint64		NetType_ = 4194304;

	/*
	2^22	4194304
	2^23	8388608
	2^24	16777216
	2^25	33554432
	2^26	67108864
	2^27	134217728
	2^28	268435456
	2^29	536870912
	2^30	1073741824
	2^31	2147483648
	2^32	4294967296
	2^33	8589934592
	2^34	17179869184
	2^35	34359738368
	2^36	68719476736
	2^37	137438953472
	2^38	274877906944
	2^39	549755813888
	2^40	1099511627776
	2^41	2199023255552
	2^42	4398046511104
	2^43	8796093022208
	2^44	17592186044416
	2^45	35184372088832
	2^46	70368744177664
	2^47	140737488355328
	2^48	281474976710656
	2^49	562949953421312
	2^50	1125899906842624
	2^51	2251799813685248
	*/

	const static uint64		EnemyProjectile_ = 0xfffffffe;
	const static uint64		PlayerProjectile_ = 0xffffffff;

	static uint64 ConvertTypeForMask(Type t)
	{
		uint64 i = static_cast<uint64>(t);
		if ( i > 1 )
		{
			i -= 1;

			// awful spikes hack
			if ( i > 10 )
				i -= 3;
		}
		else
		{
			return i;
		}

		return static_cast<uint64>(pow(2.0, (double)i));
	}

	// Anything that can be walked on
	static uint64 GeomBitMask =
		Wall_ | Platform_ | Slime_ | BouncingBoulder_ |
		Spike_ | DissapearingPlatform_ | DestructablePlatform_;

	// Anything that can wall stick to
	static uint64 WallBitMask =
		Wall_ | Platform_ | DissapearingPlatform_ | DestructablePlatform_;

	// Hazard Geom Bit Mask
	static uint64 HazardGeomBitMask =
		Spike_ | Slime_ | DropHazard_;

	static uint64 DynamicNetObject =
		EnemyBlueMinion_ | EnemyMage_ |
		EnemyDisciple_ | EnemyTaurus_ |
		EnemyShadow_ | EnemyGreenHead_ | DropHazard_;
}



// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, ID::Texture>	TextureHolder;
typedef ResourceHolder<sf::Font, ID::Font>			FontHolder;
typedef ResourceHolder<sf::SoundBuffer, ID::Audio>	SoundBufferHolder;
#endif