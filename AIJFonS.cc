#include "Player.hh"
#include <math.h>

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
  struct SoldatInfo {
    ID id, post;
    int mode;
    Info da;

    SoldatInfo() {
      id = -1;
      da = Info();
      post = -1;
      mode = MOVIMENT;
    }

    SoldatInfo(ID i) {
      id = i;
      da = Info();
      post = -1;
      mode = MOVIMENT;
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
    bool meitatNord;

    HeliInfo() {
      id = post = -1;
      da = Info();
      target = Posicio(-1, -1);
      meitatNord = false;
      mode = TRANSPORT;
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
  vector<bool> validHelis;

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

    int r = mrand.uniforme(0, nPosts - 1);
    while (infoPosts[r].da.valor != VALOR_ALT)
      r = mrand.uniforme(0, nPosts - 1);
    infoHelis[0].post = r;

    int s = mrand.uniforme(0, nPosts - 1);
    while (infoPosts[s].da.valor != VALOR_ALT or r == s)
      s = mrand.uniforme(0, nPosts - 1);
    infoHelis[1].post = s;
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

    mapa_helis(validHelis);
    for (int i = 0; i < nPosts; ++i) {
      mapa_post(p2id(infoPosts[i].da.pos), mapesSoldats[i], distanciesSoldats[i]);
      mapa_post_helis(p2id(infoPosts[i].da.pos), mapesHelis[i]);
    }
  }

  void update() {
    IDhelis = helis(equip);
    IDsoldats = soldats(equip);
    nSoldats = IDsoldats.size();
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
        infoSoldats.erase(itr++);
      } else {
        itr->second.da = dades(id);
        ++itr;
      }
    }
    /*for (auto& p : infoSoldats) {
      ID id = p.first;
      cerr << id << endl;
      if (not soldat_viu(id)) {
        cerr << id << " died" << endl;
        infoSoldats.erase(id);
      } else {
        cerr << id << " we copy you" << endl;
        infoSoldats[id].da = dades(id);
      }
    }*/

    for (auto& id : IDsoldats) {
      cerr << "before count" << endl;
      if (infoSoldats.count(id) == 0) {
        cerr << id << " just enroled!" << endl;
        Soldat s = Soldat(id, SoldatInfo(id));
        init_soldat(s);
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
              if (valid(i + k, j + l) and que(i + k, j + l) == MUNTANYA) {
                mapa[c2id(i, j)] = false;
                trobat = true;
              }
            }
          }
        }
      }
    }
  }

  void visitar_posicio_mapa_helis(int instruccio, int id, vector<bool> & visitat, vector<int> & mapa, queue<int> &q) {
    if (not visitat[id] and validHelis[id % nMapa]) {
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

      visitar_posicio_mapa_helis(RELLOTGE, po2id(pos, (o + 1) % 4), visitat, mapa, q);

      visitar_posicio_mapa_helis(CONTRA_RELLOTGE, po2id(pos, (o - 1 + 4) % 4), visitat, mapa, q);

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

  void init_soldat(Soldat &s) {
    int soldatsPerPost = nSoldats / nPosts;
    int soldatsPerPostAlt = soldatsPerPost + ceil((float) (nSoldats % nPosts) / (float) nPostsAlts);
    for (int i = 0; i < nPosts; ++i) {
      if (infoPosts[i].n < soldatsPerPost or (infoPosts[i].da.valor == VALOR_ALT and infoPosts[i].n < soldatsPerPostAlt)) {
        ++(infoPosts[i].n);
        s.second.post = infoPosts[i].id;
        break;
      }
    }
    s.second.da = dades(s.second.id);
  }

  void logica_soldat(SoldatInfo &s) {

  }

  void logica_heli(HeliInfo & h) {
    if (h.da.paraca.size() > 0) {
      for (auto& p : h.da.paraca) {
        Posicio baixaPos = Posicio(h.da.pos.x + mrand.uniforme(-2, 2), h.da.pos.y + mrand.uniforme(-2, 2));
        if (temps_foc(baixaPos.x, baixaPos.y) == 0 and valid_per_soldat(p2id(baixaPos))) {
          ordena_paracaigudista(baixaPos.x, baixaPos.y);
        }
      }
    }
  }

  void mou_soldat(const SoldatInfo &s) {
    Posicio nextPos;
    switch (s.mode) {
    case MOVIMENT:
      log("moviment");
      cerr << s.post << endl;
      nextPos = id2p(mapesSoldats[s.post][p2id(s.da.pos)]);
      log("1");
      ID nextSoldat = quin_soldat(nextPos.x, nextPos.y);
      if (nextSoldat > 0 and dades(nextSoldat).equip == equip) {
        log("2");
        int deltaX = nextPos.x - s.da.pos.x;
        int deltaY = nextPos.y - s.da.pos.y;

        deltaX = mrand.uniforme(0, 1) > 0 ? deltaX : -deltaX;
        deltaY = mrand.uniforme(0, 1) > 0 ? deltaY : -deltaY;
        nextPos = Posicio(s.da.pos.x + deltaX, s.da.pos.y + deltaY);
      }
      break;
    }

    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        ID id = quin_soldat(s.da.pos.x + i, s.da.pos.y + j);
        if (id > 0 and dades(id).equip != equip) {
          nextPos = Posicio(s.da.pos.x + i, s.da.pos.y + j);
        }
      }
    }
    log("3");
    ordena_soldat(s.id, nextPos.x, nextPos.y);
    log("end");
  }

  void mou_helicopter(const HeliInfo &h) {
    ordena_helicopter(h.id, mapesHelis[h.post][po2id(h.da.pos, h.da.orientacio)]);
  }

  void log(string s) {
    cerr << s << endl;
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
    log("before soldiers");
    for (auto& p : infoSoldats) {
      mou_soldat(p.second);
    }
    log("after soldiers");
    for (auto& h : infoHelis) {
      logica_heli(h);
      mou_helicopter(h);
    }
    log("after choppers");
  }
}
;

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

