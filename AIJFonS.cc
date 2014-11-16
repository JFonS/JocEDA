#include "Player.hh"

using namespace std;

/**
 * Escriu el nom * del teu jugador i guarda 
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME JFonS

/**
 * Podeu declarar constants aquí
 */

//Estats
const int MOVIMENT = 0;
const int DEFENSA = 1;
const int ATAC = 2;
const int TRANSPORT = 3;


const int nHelis = 2;
const int nMapa = MAX * MAX;

typedef int ID;

struct PLAYER_NAME: public Player {

	/**
	 * Factory: retorna una nova instància d'aquesta classe.
	 * No toqueu aquesta funció.
	 */
	static Player* factory() {
		return new PLAYER_NAME;
	}

	/**
	 * Structs
	 */
	struct Soldat {
		ID id;
		ID post;
		int mode = MOVIMENT;
		Info dades;
	};

	/**
	 * Els atributs dels vostres jugadors es poden definir aquí.
	 */
	Posicio id2p(ID i) {
		i %= nMapa;
		return Posicio(i / MAX, i % MAX);
	}
	Posicio c2p(int x, int y) {
		return Posicio(x, y);
	}
	ID p2id(Posicio p) {
		return p.x * MAX + p.y;
	}
	ID c2id(int x, int y) {
		return x * MAX + y;
	}
	int id2o(int id) {
		return id / nMapa;
	}
	int po2id(Posicio p, int o) {
		return p2id(p) + o * nMapa;
	}

	bool valid_per_soldat(int id) {
		Posicio p = id2p(id);
		return (que(p.x, p.y) == GESPA or que(p.x, p.y) == BOSC);
	}

	int equip;
	vector<ID> IDsoldats, IDhelis;
	int nSoldats;

	map<ID, Soldat> infoSoldats;

	vector<int> enemics;
	vector<vector<ID>> soldatsEnemics;
	vector<vector<ID>> helisEnemics;
	vector<Post> infoPosts;
	int nPosts;

	vector<vector<int>> mapesSoldats;
	vector<vector<int>> distanciesSoldats;
	vector<vector<int>> mapesHelis;
	vector<bool> validHelis;

	/**
	 * Dades
	 */
	void init() {
		equip = qui_soc();
		soldatsEnemics = vector<vector<ID>>(3);
		helisEnemics = vector<vector<ID>>(3);
		update();
		nPosts = infoPosts.size();
		init_mapes();
	}

	void update() {
		infoPosts = posts();
		IDsoldats = soldats(equip);
		IDhelis = helis(equip);
		nSoldats = IDsoldats.size();
		update_enemics();
	}

	void update_enemics() {
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
		if (equip != 4)
			enemics[2] = 4;
	}

	/**
	 * Mapes
	 */
	void mapa_helis(vector<bool> & mapa) {
		mapa = vector<bool>(nMapa, true);
		for (int i = 0; i < MAX; ++i) {
			for (int j = 0; j < MAX; ++j) {
				if (que(i, j) == MUNTANYA)
					mapa[c2id(i, j)] = false;
				else {
					bool trobat = false;
					for (int k = -2; k <= 2 and not trobat; ++k) {
						for (int l = -2; l <= 2 and not trobat; ++l) {
							if (valid(i + k, j + l)
									and que(i + k, j + l) == MUNTANYA) {
								mapa[c2id(i, j)] = false;
								trobat = true;
							}
						}
					}
				}
			}
		}
	}

	void visitar_posicio_mapa_helis(int instruccio, int id,
			vector<bool> & visitat, vector<int> & mapa, queue<int> &q) {
		if (not visitat[id] and validHelis[id%nMapa]) {
			visitat[id] = true;
			q.push(id);
			mapa[id] = instruccio;
		}

	}

	void mapa_post_helis(int post, vector<int> & mapa) {
		int dx[4] = { -1, 0, 1, 0 };
		int dy[4] = { 0, -1, 0, 1 };
		mapa = vector<int>(nMapa * 4);
		vector<bool> visitat = vector<bool>(nMapa * 4, false);
		queue<int> q;
		visitat[post] = true;
		q.push(post);
		mapa[post] = post;

		while (!q.empty()) {
			int id = q.front();
			Posicio pos = id2p(id);
			int o = id2o(id);
			q.pop();

			Posicio p = Posicio(pos.x + dx[o], pos.y + dy[o]);
			if (valid(p))
				visitar_posicio_mapa_helis(AVANCA1, po2id(p, o), visitat, mapa, q);

			p = Posicio(p.x + dx[o], p.y + dy[o]);
			if (valid(p))
				visitar_posicio_mapa_helis(AVANCA2, po2id(p, o), visitat, mapa, q);

			visitar_posicio_mapa_helis(RELLOTGE, po2id(pos, (o + 1) % 4),
					visitat, mapa, q);

			visitar_posicio_mapa_helis(CONTRA_RELLOTGE, po2id(pos, (o - 1 + 4) % 4),
					visitat, mapa, q);

		}
	}

	void mapa_post(int primera_id, vector<int> & mapa,
			vector<int> & distancia) {
		mapa = vector<int>(nMapa);
		distancia = vector<int>(nMapa);
		vector<bool> visitat = vector<bool>(nMapa, false);
		queue<int> q;

		visitat[primera_id] = true;
		q.push(primera_id);
		mapa[primera_id] = primera_id;
		distancia[primera_id] = 0;

		while (!q.empty()) {
			int id = q.front();
			Posicio pos = id2p(id);
			q.pop();
			for (int i = -1; i <= 1; ++i) {
				for (int j = -1; j <= 1; ++j) {
					int novaId = p2id(Posicio(pos.x + i, pos.y + j));
					if (not visitat[novaId] and valid_per_soldat(novaId)) {
						visitat[novaId] = true;
						q.push(novaId);
						mapa[novaId] = id;
						distancia[novaId] = distancia[id] + 1;
					}
				}
			}
		}
	}

	void init_mapes() {
		int n = infoPosts.size();
		mapesSoldats = vector<vector<int>>(n);
		distanciesSoldats = vector<vector<int>>(n);

		mapesHelis = vector<vector<int>>(n);

		mapa_helis(validHelis);

		for (int i = 0; i < n; ++i) {
			mapa_post(p2id(infoPosts[i].pos), mapesSoldats[i],
					distanciesSoldats[i]);
			mapa_post_helis(p2id(infoPosts[i].pos), mapesHelis[i]);
		}

		for (int i = 0; i < MAX; ++i) {
			for (int j = 0; j < MAX; ++j) {
				cerr << validHelis[c2id(i, j)] << " ";
			}
			cerr << endl;
		}
	}

	/**
	 * Mètode play.
	 *
	 * Aquest mètode serà invocat una vegada cada torn.
	 */
	virtual void play() {
		if (quin_torn() == 0)
			init();
		else
			update();

		for (int i = 0; i < nSoldats; ++i) {
			Info da = dades(IDsoldats[i]);
			Posicio nextPos = id2p(mapesSoldats[i%nPosts][p2id(da.pos)]);
			ordena_soldat(IDsoldats[i], nextPos.x, nextPos.y);
		}

		Info da = dades(IDhelis[0]);
		ordena_helicopter(IDhelis[0], mapesHelis[7][po2id(da.pos,da.orientacio)]);
		da = dades(IDhelis[1]);
				ordena_helicopter(IDhelis[1], mapesHelis[15][po2id(da.pos,da.orientacio)]);
		/*for (int i = 0; i < nHelis; ++i) {
			Info da = dades(IDhelis[i]);
			ordena_helicopter(IDhelis[i], mapesHelis[i][po2id(da.pos,da.orientacio)]);
		}*/
	}

}
;

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

