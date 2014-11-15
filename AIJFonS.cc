#include "Player.hh"

using namespace std;

/**
 * Escriu el nom * del teu jugador i guarda 
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME JFonS

typedef int ID;

/**
 * Podeu declarar constants aquí
 */

struct Grup {
	Posicio pos;
	int n;
};

struct PLAYER_NAME: public Player {

	/**
	 * Factory: retorna una nova instància d'aquesta classe.
	 * No toqueu aquesta funció.
	 */
	static Player* factory() {
		return new PLAYER_NAME;
	}

	/**
	 * Els atributs dels vostres jugadors es poden definir aquí.
	 */
	vector<ID> IDsoldats;
	vector<ID> IDhelis;

	int equip;
	vector<int> enemics;
	vector< vector<ID>> soldatsEnemics;
	vector< vector<ID>> helisEnemics;

	Grup grupA, grupB;

	void init() {
		equip = qui_soc();
		init_enemics();

		grupA.pos = Posicio(2,2);
		grupB.pos = Posicio(32,2);

		IDsoldats = soldats(equip);
		IDhelis = helis(equip);
	}

	void init_enemics() {
		enemics = vector<int>(3);
				int c = 0;
				for (int i = 1; i < 5; ++i) {
					if (i != equip) {
						enemics[c] = i;
						//soldatsEnemics[c] = soldats(i);
						//helisEnemics[c] = helis(i);
						++c;
					}
				}
		if (equip != 4)enemics[2] = 4;
	}


	void reubica_grup(Grup &g) {
		int x = g.pos.x;
		int y = g.pos.y;
		for (int i = -2; i < 3; ++i) {
			for (int j = -2; j < 3; ++j) {
				if (quin_soldat(x + i, y + j) > 0) {
					g.pos.x += x + i;
					g.pos.y += y + j;
					++g.n;
				}
			}
		}
		g.pos.x /= g.n;
		g.pos.y /= g.n;
		valid()
	}

	/**
	 * Mètode play.
	 *
	 * Aquest mètode serà invocat una vegada cada torn.
	 */
	virtual void play() {
		if (quin_torn() == 0)
			init();
		for (int i = 0; i < IDsoldats.size(); ++i) {
			Info da = dades(IDsoldats[i]);
			ordena_soldat(IDsoldats[i], da.pos.x + 1, da.pos.y + 1);
		}
		for (int i = 0; i < IDhelis.size(); ++i) {
			Info da = dades(IDhelis[i]);
			ordena_helicopter(IDhelis[i], NAPALM);

		}
	}

};

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

