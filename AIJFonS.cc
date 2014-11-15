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

	void log(string s) {cerr << "JFLog: " << s << endl; }

	/**
	 * Els atributs dels vostres jugadors es poden definir aquí.
	 */
	vector<ID> IDsoldats;
	vector<ID> IDhelis;

	Posicio id2p (ID i) { return Posicio(i/MAX,i%MAX); }
	Posicio c2p (int x, int y) { return Posicio(x,y); }
	ID p2id (Posicio p) { return p.x*MAX + p.y; }
	ID c2id (int x, int y) { return x*MAX + y; }
	bool valid_per_soldat(int id) {
		Posicio p = id2p(id);
		return (que(p.x,p.y) == GESPA or que(p.x,p.y) == BOSC);
	}

	int equip;
	vector<int> enemics;
	vector< vector<ID>> soldatsEnemics;
	vector< vector<ID>> helisEnemics;
	vector<Post> infoPosts;
	vector<vector<int>> mapesSoldats;


	void init() {
		equip = qui_soc();
		infoPosts = posts();
		IDsoldats = soldats(equip);
		IDhelis = helis(equip);
		init_enemics();
		init_mapes();
	}


	void mapa_post(int primera_id, vector<int> & mapa) {
		mapa = vector<int>(MAX*MAX);
	    vector<bool> visitat = vector<bool>(MAX*MAX, false);
	    queue<int> q;

	    visitat[primera_id] = true;
	    q.push(primera_id);
	    mapa[primera_id] = primera_id;

	    while(!q.empty()) {
	    	int id = q.front();
	    	Posicio pos = id2p(id);
	        q.pop();
	        for (int i = -1; i <= 1; ++i) {
	        	for (int j = -1; j <= 1; ++j) {
	        		int novaId = p2id(Posicio(pos.x + i, pos.y + j));
	        		if (valid_per_soldat(novaId) and not visitat[novaId]){
	        			visitat[novaId] = true;
	        			q.push(novaId);
	        			mapa[novaId] = id;
	        		}
	        	}
	        }
	    }
	}

	void init_mapes() {
		int n = infoPosts.size();
		mapesSoldats = vector<vector<int>>(n);
		for (int i = 0; i < n; ++i) mapa_post(p2id(infoPosts[i].pos), mapesSoldats[i]);
	}

	void init_enemics() {
		soldatsEnemics = vector< vector<ID>>(3);
		helisEnemics = vector< vector<ID>>(3);
		enemics = vector<int>(3);
				int c = 0;
				for (int i = 1; i < 5; ++i) {
					if (i != equip) {
						enemics[c] = i;
						soldatsEnemics[c] = soldats(i);
						helisEnemics[c] = helis(i);
						++c;
					}
				}
		if (equip != 4)enemics[2] = 4;
	}

	/**
	 * Mètode play.
	 *
	 * Aquest mètode serà invocat una vegada cada torn.
	 */
	virtual void play() {
		if (quin_torn() == 0)
			init();

		/*Info da = dades(IDsoldats[0]);
		Posicio nextPos = id2p(mapesSoldats[0][p2id(da.pos)]);
		ordena_soldat(IDsoldats[0], nextPos.x, nextPos.y);*/


		for (int i = 0; i < IDsoldats.size(); ++i) {
			Info da = dades(IDsoldats[i]);
			Posicio nextPos = id2p(mapesSoldats[i%mapesSoldats.size()][p2id(da.pos)]);
			ordena_soldat(IDsoldats[i], nextPos.x, nextPos.y);
		}
		/*for (int i = 0; i < IDhelis.size(); ++i) {
			Info da = dades(IDhelis[i]);
			ordena_helicopter(IDhelis[i], NAPALM);

		}*/
	}

};

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

