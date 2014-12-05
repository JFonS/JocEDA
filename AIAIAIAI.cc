#include "Player.hh"
#include <math.h>

using namespace std;

/**
 * Escriu el nom * del teu jugador i guarda 
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME AIAIAI

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
	 struct SoldatInfo {
	 	ID id, post;
	 	int mode;
	 	Info da;
	 	Posicio nextMove;

	 	SoldatInfo() {
	 		id = -1;
	 		da = Info();
	 		post = -1;
	 		mode = MOVIMENT;
	 		nextMove = Posicio();
	 	}

	 	SoldatInfo(ID i) {
	 		id = i;
	 		da = Info();
	 		post = -1;
	 		mode = MOVIMENT;
	 		nextMove = Posicio();
	 	}
	 };

	 struct PostInfo {
	 	ID id;
	 	Post da;
	 	int n;

	 	PostInfo() {
	 		id = -1;
	 		da = Post();
	 		n = 0;
	 	}
	 };

	 struct HeliInfo {
	 	ID id;
	 	Info da;
	 	Posicio target;
	 	int post;
	 	int mode;
	 	int index;
	 	bool meitatNord;

	 	HeliInfo() {
	 		id = post = -1;
	 		da = Info();
	 		target = Posicio(-1, -1);
	 		meitatNord = false;
	 		mode = TRANSPORT;
	 		index = -1;
	 	}
	 };

	 typedef pair<ID, SoldatInfo> Soldat;

	 atzar mrand;

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
	 bool soldat_viu(ID id) {
	 	for (auto& i : IDsoldats) {
	 		if (i == id)
	 			return true;
	 	}
	 	return false;
	 }
	 int randomPost() {
	 	return mrand.uniforme(0, nPosts - 1);
	 }

	 int equip;
	 vector<ID> IDsoldats, IDhelis;
	 int nSoldats;

	 vector<HeliInfo> infoHelis;
	 map<ID, SoldatInfo> infoSoldats;
	 vector<PostInfo> infoPosts;
	 int nPosts;
	 int nPostsAlts;

	 vector<int> enemics;
	 vector<vector<ID>> soldatsEnemics;
	 vector<vector<ID>> helisEnemics;

	 vector<vector<int>> mapesSoldats;
	 vector<vector<int>> distanciesSoldats;
	 vector<vector<int>> mapesHelis;
	 vector<vector<bool>> validHeli;

	/**
	 * Dades
	 */
	 void init() {
	 	mrand = atzar(time(0));
	 	infoSoldats = map<ID, SoldatInfo>();
	 	infoHelis = vector<HeliInfo>(nHelis);
	 	soldatsEnemics = vector<vector<ID>>(3);
	 	helisEnemics = vector<vector<ID>>(3);
	 	equip = qui_soc();
	 	init_posts();
	 	init_info_helis();
	 	init_enemics();
	 	init_mapes();
	 }

	 void init_info_helis() {
	 	IDhelis = helis(equip);
	 	infoHelis[0].id = IDhelis[0];
	 	infoHelis[1].id = IDhelis[1];
	 	infoHelis[0].meitatNord = true;

	 	int r = randomPost();
	 	while (infoPosts[r].da.valor != VALOR_ALT)
	 		r = randomPost();
	 	infoHelis[0].post = r;
	 	infoHelis[0].target = infoPosts[r].da.pos;

	 	int s = randomPost();
	 	while (infoPosts[s].da.valor != VALOR_ALT or r == s)
	 		s = randomPost();
	 	infoHelis[1].post = s;
	 	infoHelis[1].target = infoPosts[s].da.pos;

	 	infoHelis[0].index = 0;
	 	infoHelis[1].index = 1;
	 }

	 void init_enemics() {
	 	enemics = vector<int>(3);
	 	int c = 0;
	 	for (int i = 1; i < 5; ++i) {
	 		if (i != equip) {
	 			enemics[c] = i;
	 			++c;
	 		}
	 	}
	 	if (equip != 4)
	 		enemics[2] = 4;
	 }

	 void init_posts() {
	 	vector<Post> vecPosts = posts();
	 	nPosts = vecPosts.size();
	 	infoPosts = vector<PostInfo>(nPosts);
	 	nPostsAlts = 0;
	 	for (int i = 0; i < nPosts; ++i) {
	 		infoPosts[i].id = i;
	 		if (vecPosts[i].valor == VALOR_ALT)
	 			++nPostsAlts;
	 	}
	 	update_posts();
	 }

	 void init_mapes() {

	 	mapesSoldats = vector<vector<int>>(nPosts);
	 	distanciesSoldats = vector<vector<int>>(nPosts);

	 	mapesHelis = vector<vector<int>>(nPosts);

	 	mapa_helis(validHeli);
	 	for (int i = 0; i < nPosts; ++i) {
	 		mapa_post(p2id(infoPosts[i].da.pos), mapesSoldats[i], distanciesSoldats[i]);
	 	}
	 }

	 void update() {
	 	IDhelis = helis(equip);
	 	IDsoldats = soldats(equip);
	 	nSoldats = IDsoldats.size();
	 	mapa_helis(validHeli);
	 	update_posts();
	 	update_enemics();
	 	update_info_soldats();
	 	update_info_helis();
	 }

	 void update_posts() {
	 	vector<Post> tempPosts = posts();
	 	for (int i = 0; i < nPosts; ++i) {
	 		infoPosts[i].da = tempPosts[i];
	 	}
	 }

	 void update_info_soldats() {
	 	auto itr = infoSoldats.begin();
	 	while (itr != infoSoldats.end()) {
	 		ID id = itr->first;
	 		if (not soldat_viu(id)) {
	 			--infoPosts[itr->second.post].n;
	 			infoSoldats.erase(itr++);
	 		} else {
	 			itr->second.da = dades(id);
	 			++itr;
	 		}
	 	}

	 	for (auto& id : IDsoldats) {
	 		if (infoSoldats.count(id) == 0) {
	 			Soldat s = Soldat(id, SoldatInfo(id));
	 			init_soldat(s.second);
	 			infoSoldats.insert(s);
	 		}
	 	}
	 	log("jobs done");
	 }

	 void update_info_helis() {
	 	for (auto& heli : infoHelis) {
	 		heli.da = dades(heli.id);
	 	}
	 }

	 void update_enemics() {
	 	for (int i = 0; i < 3; ++i) {
	 		soldatsEnemics[i] = soldats(enemics[i]);
	 		helisEnemics[i] = helis(enemics[i]);
	 	}
	 }

	/**
	 * Mapes
	 */

	 void mapa_helis(vector<vector<bool>> & mapa) {
	 	mapa = vector<vector<bool>>(2, vector<bool>(nMapa, true));

	 	for (int i = 0; i < MAX; ++i) {
	 		for (int j = 0; j < MAX; ++j) {
	 			if (que(i, j) == MUNTANYA) {
	 				mapa[0][c2id(i, j)] = false;
	 				mapa[1][c2id(i, j)] = false;
	 			} else {
	 				bool trobat = false;
	 				for (int k = -2; k <= 2 and not trobat; ++k) {
	 					for (int l = -2; l <= 2 and not trobat; ++l) {
	 						if (valid(i + k, j + l) and que(i + k, j + l) == MUNTANYA) {
	 							mapa[0][c2id(i, j)] = false;
	 							mapa[1][c2id(i, j)] = false;
	 							trobat = true;
	 						}
	 					}
	 				}
	 			}
	 		}
	 	}

	 	for (int i = 1; i <= 4; ++i) {
	 		vector<int> h = helis(i);
	 		for (int j = 0; j < h.size(); ++j) {
	 			Info ih = dades(h[j]);
	 			for (int k = -4; k <= 4; ++k) {
	 				for (int l = -4; l <= 4; ++l) {
	 					if (valid(ih.pos.x + k, ih.pos.y + l)) {
	 						if (ih.id != IDhelis[0])
	 							mapa[0][c2id(ih.pos.x + k, ih.pos.y + l)] = false;
	 						if (ih.id != IDhelis[1])
	 							mapa[1][c2id(ih.pos.x + k, ih.pos.y + l)] = false;
	 					}
	 				}
	 			}
	 		}
	 	}
	 }

	 void visitar_posicio_mapa_helis(int hId, int instruccio, int id, vector<bool> & visitat, vector<int> & mapa, queue<int> &q) {
	 	if (not visitat[id] and validHeli[hId][id % nMapa]) {
	 		visitat[id] = true;
	 		q.push(id);
	 		mapa[id] = instruccio;
	 	}

	 }

	 void mapa_post(int primera_id, vector<int> & mapa, vector<int> & distancia) {
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

	/**
	 *  Control
	 */
	 void init_soldat(SoldatInfo &s) {
	 	s.da = dades(s.id);
	 	typedef pair<int, int> P;
	 	priority_queue<P, vector<P>, greater<P> > q = priority_queue<P, vector<P>, greater<P> >();

	 	for (int i = 0; i < nPosts; ++i) {
	 		PostInfo post = infoPosts[i];
			//cerr << post.id << " -> " << post.n << "     " << distanciesSoldats[post.id][p2id(s.da.pos)] << endl;
	 		q.push(pair<int, int>(distanciesSoldats[post.id][p2id(s.da.pos)], post.id));
	 	}

	 	int mesProper = q.top().second;
	 	if (quin_torn() == 0 or infoPosts[mesProper].da.equip == equip) {
	 		while (infoPosts[q.top().second].n >= 2 and not q.empty())
	 			q.pop();
	 		if (not q.empty())
	 			mesProper = q.top().second;
	 	}
	 	s.post = mesProper;
	 	++infoPosts[mesProper].n;
	 }

	 Posicio hi_ha_hei_amb_napalm(int x, int y) {
	 	for (int i = -2; i <= 2; ++i) {
	 		for (int j = -2; j <= 2; ++j) {
	 			ID heli;
	 			if (valid(x,y) and (heli =quin_heli(x,y)) > 0) {
	 				Info da = dades(heli);
	 				if (da.equip != equip and da.napalm < 15) return Posicio(x,y);
	 				else return Posicio(-1,-1);
	 			}
	 		}
	 	}
	 }

	int preferencia_moviment(SoldatInfo &s, int x, int y) { //Retorna la prioritat de la casella x, y
		logPos("", c2p(x, y));

		if (temps_foc(x, y) > 0) // || hi_ha_hei_amb_napalm(x,y).x != -1)
return 0;
if (quin_soldat(x, y) > 0) {
	log("Soldat trobat");
	ID id = quin_soldat(x, y);
	if (id > 0 and dades(id).equip != equip) {
		log("Enemic");
		return 4;
	} else {
		log("Aliat");
		return mrand.uniforme(0, 1);
	}
}
if (de_qui_post(x, y) == -1 or (de_qui_post(x, y) > 0 and not de_qui_post(x, y) == equip))
	return 3;
Posicio cami;
if (quin_torn() < 150)
	cami = bfs_soldat_to_soldat(c2id(x,y));
else 
	cami = id2p(mapesSoldats[s.post][p2id(s.da.pos)]);
if (cami.x == x and cami.y == y)
	return 2;
if (valid_per_soldat(c2id(x, y)))
	return 1;
return 0;

}

void logica_soldat(SoldatInfo &s) {

	int maxPrioritat = -99999;
	Posicio pos = s.da.pos;
	Posicio cami = id2p(mapesSoldats[s.post][p2id(s.da.pos)]);
	logPos("at", pos);

	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (i != 0 or j != 0) {
				Posicio p = Posicio(pos.x + i, pos.y + j);
				int prioritat = preferencia_moviment(s, p.x, p.y);
				if (prioritat > maxPrioritat) {
					maxPrioritat = prioritat;
					s.nextMove = p;
				}
			}
		}
	}

	logPos("nextmove:", s.nextMove);
}

void logica_heli(HeliInfo & h) {
	if (not validHeli[h.index][p2id(h.target)]) {
		int r = randomPost();
		int i = 50;
		while (i < 50 and (infoPosts[r].da.valor != VALOR_ALT or not validHeli[h.index][p2id(infoPosts[r].da.pos)])) {
			r = randomPost();
			++i;
		}
		h.post = r;
		h.target = infoPosts[r].da.pos;
	}

	if (p2id(h.da.pos) == p2id(infoPosts[h.post].da.pos)) {
		h.post = mrand.uniforme(0, nPosts - 1);
		h.target = infoPosts[h.post].da.pos;
	}

	int enemics = 0;
	for (int i = -2; i <= 2; ++i) {
		for (int j = -2; j <= 2; ++j) {
			ID id = quin_soldat(h.da.pos.x + i, h.da.pos.y + j);
			if (id > 0 and dades(id).equip != equip)
				++enemics;
		}
	}

	if (enemics > 4 and h.da.napalm == 0)
		ordena_helicopter(h.id, NAPALM);

	if (h.da.paraca.size() > 0) {
		for (auto& p : h.da.paraca) {
			Posicio baixaPos = Posicio(h.da.pos.x + mrand.uniforme(-2, 2), h.da.pos.y + mrand.uniforme(-2, 2));
			if (temps_foc(baixaPos.x, baixaPos.y) == 0 and valid_per_soldat(p2id(baixaPos)) and quin_soldat(baixaPos.x, baixaPos.y) < 1) {
				ordena_paracaigudista(baixaPos.x, baixaPos.y);
			}
		}
	}
}

void mou_soldat(const SoldatInfo &s) {
	ordena_soldat(s.id, s.nextMove.x, s.nextMove.y);
}

bool visitar_posicio_bfs_helis(int hId, int instruccio, int id, vector<bool> & visitat, queue<int> &q, int &inici) {
	if (not visitat[id] and validHeli[hId][id % nMapa]) {
		visitat[id] = true;
		q.push(id);
		if (id == inici) {
			return true;
		}
	}
	return false;
}

int bfs_heli(int hId, Posicio posInici, int orientacioInici, Posicio posDesti) {

	int inici = po2id(posInici, orientacioInici);
	int desti = p2id(posDesti);

	int dx[4] = { -1, 0, 1, 0 };
	int dy[4] = { 0, -1, 0, 1 };
	vector<bool> visitat = vector<bool>(nMapa * 4, false);
	queue<int> q;
	visitat[desti] = true;

	q.push(desti);

	while (!q.empty()) {
		int id = q.front();
		Posicio pos = id2p(id);
		int o = id2o(id);
		q.pop();
		Posicio p = Posicio(pos.x + dx[o], pos.y + dy[o]);
		if (valid(p))
			if (visitar_posicio_bfs_helis(hId, AVANCA1, po2id(p, o), visitat, q, inici)) {
				return AVANCA1;
			}

			p = Posicio(p.x + dx[o], p.y + dy[o]);
			if (valid(p))
				if (visitar_posicio_bfs_helis(hId, AVANCA1, po2id(p, o), visitat, q, inici))
					return AVANCA2;

				if (visitar_posicio_bfs_helis(hId, AVANCA1, po2id(pos, (o + 1) % 4), visitat, q, inici))
					return RELLOTGE; 

				if (visitar_posicio_bfs_helis(hId, CONTRA_RELLOTGE, po2id(pos, (o - 1 + 4) % 4), visitat, q, inici))
					return CONTRA_RELLOTGE;
			}
			return -1;
		}

		void mou_helicopter(const HeliInfo &h) {

			ordena_helicopter(h.id, bfs_heli(h.index, h.da.pos, h.da.orientacio, h.target));
		}

		int bfs_soldat_to_soldat(int posActual) {
			vector<bool> visitat = vector<bool>(nMapa, false);
			queue<int> q;

			visitat[posActual] = true;
			q.push(posActual);

			while (!q.empty()) {
				int id = q.front();
				Posicio pos = id2p(id);
				q.pop();
				for (int i = -1; i <= 1; ++i) {
					for (int j = -1; j <= 1; ++j) {
						Posicio novaPos = Posicio(pos.x + i, pos.y + j);
						int novaId = p2id(novaPos);
						//cerr << "Mirant pos (" << novaPos.x << "," << novaPos.y << ") -> " << novaId << endl;
						if (not visitat[novaId] and valid_per_soldat(novaId)) {
							visitat[novaId] = true;
							q.push(novaId);
							ID sol = quin_soldat(novaPos.x, novaPos.y);
							if(sol > 0 and dades(sol).equip != equip) {
								cerr << "L'hem tobat, tornant..." << endl;
								return bfs_soldat_tornada(novaId, posActual, visitat);
							}
						}
					}
				}
			}
		}

		int bfs_soldat_tornada(int posIicial, int posFinal, vector<bool> &jaVisitats) {
			vector<bool> visitat = vector<bool>(nMapa, false);
			queue<int> q;

			visitat[posFinal] = true;
			q.push(posFinal);

			while (!q.empty()) {
				int id = q.front();
				Posicio pos = id2p(id);
				q.pop();
				for (int i = -1; i <= 1; ++i) {
					for (int j = -1; j <= 1; ++j) {
						Posicio novaPos = Posicio(pos.x + i, pos.y + j);
						int novaId = p2id(novaPos);
						if (jaVisitats[novaId] and not visitat[novaId]) {
							visitat[novaId] = true;
							q.push(novaId);
							if(novaId == posIicial) {
								cerr << "Ja hem tornat, venim de la pos " << id << endl;
								return id;
							}
						}
					}
				}
			}
		}

		void reassigna_soldats() {
			map<ID,SoldatInfo>::iterator itA, itB;
			for (itA = infoSoldats.begin(); itA != infoSoldats.end(); ++itA) {
				SoldatInfo sA = itA->second;
				for (itB = infoSoldats.begin(); itB != infoSoldats.end(); ++itB) {
					SoldatInfo sB = itB->second;
					if (sA.id != sB.id) {
						int distActual = distanciesSoldats[sA.post][p2id(sA.da.pos)] + distanciesSoldats[sB.post][p2id(sB.da.pos)];
						int distCanvi = distanciesSoldats[sA.post][p2id(sB.da.pos)] + distanciesSoldats[sB.post][p2id(sA.da.pos)];
						if (distCanvi < distActual) {
							int temp = itB->second.post;
							itB->second.post = itA->second.post;
							itA->second.post = temp;

						}
					}
				}
			}
		}

		void log(string s) {
		//cerr << s << endl;
		}

		void logPos(string s, Posicio p) {
		//cerr << s << "  (" << p.x << "," << p.y << ")" << endl;
		}

	/**
	 * Mètode play.
	 *
	 * Aquest mètode serà invocat una vegada cada torn.
	 */
	 virtual void play() {
	 	if (quin_torn() == 0)
	 		init();

	 	update();
	 	reassigna_soldats();

	 	log("before soldiers");
	 	for (auto& p : infoSoldats) {
	 		logica_soldat(p.second);
	 		mou_soldat(p.second);
	 	}
	 	log("after soldiers");
	 	for (auto& h : infoHelis) {
	 		logica_heli(h);
	 		mou_helicopter(h);
	 	}
	 	log("after choppers");
	 }
	};

/**
 * No toqueu aquesta línia.
 */
 RegisterPlayer(PLAYER_NAME);
