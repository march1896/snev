#include <list>
template<T>
class Automaton {
 public:
  typedef T SymbolType;

  struct Edge {
    State const * From;
    State const * To;
    SymbolType    Id;
  };

  struct State { 
    std::list<Edge*> Outs;
  };

  std::list<State>  States;
  std::set<Edge>    Edges;
  State*            Start;
  std::list<State*> Accepts;

 public:
  Automaton() {
    Init();
  }

  Automaton(Automaton const & from) {
    this->States = from.States;

    std::map<State const *, State const *> sMap;
    for (std::list<State>::iterator s = from.States.begin(), d = States.begin(); 
         s != from.States.end(); s ++, d ++) {
      sMap[&(*s)] = &(*d);
    }

    for (std::list<State>::iterator s = from.States.begin(); s != from.States.end(); s ++) {
      for (std::list<Edge*>::iterator e = s->Outs.begin(); e != s->Outs.end(); e ++) {
        std::pair<std::set<Edge>::iterator, bool> res = this->Edges.insert(Edge(sMap[e->From], sMap[e->To], e->SymbolType));
        sMap[&(*s)]->insert(&(*res.first));
      }
    }

    assert(this->Edges.size() == from.Edges.size());
    this->Start = sMap[from.Start];
    for (auto itr = from.Accepts.begin(); itr != from.Accepts.end(); itr ++) {
      this->Accepts.insert(sMap[*itr]);
    }
  }

  ~Automaton() {
    Clear();
  }

  void Init() {
  }

  void Clear() {
  }
};

template<T>
class NFA {
  Automaton<T> Atm;

 public:
  NFA();

};
