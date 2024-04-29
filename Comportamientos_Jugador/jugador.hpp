#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct stateN0{
  ubicacion jugador;
  ubicacion colaborador;
  Action ultimaOrdenColaborador;

  //si la ubicación del jugador y del colaborador es la misma
  bool operator==(const stateN0 &x) const{
    return (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula &&
    colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c);
  }

  bool operator!=(const stateN0 &x) const{
    return (jugador.f != x.jugador.f || jugador.c != x.jugador.c || jugador.brujula != x.jugador.brujula ||
    colaborador.f != x.colaborador.f || colaborador.c != x.colaborador.c);
  }
};

/** Definición del tipo nodo del nivel 0 */
struct nodeN0{
    stateN0 st; //ubicaciones jugador y colaborador y ult acción colaborador
    list<Action> secuencia;

    bool operator==(const nodeN0 &n) const { //dos nodos son iguales si sus estados son los mismos
        return (st == n.st);
    }

    bool operator<(const nodeN0 &b) const {
        if (st.jugador.f < b.st.jugador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f and st.jugador.c < b.st.jugador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f and st.jugador.c == b.st.jugador.c and st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else
            return false;
    }
};


/** Definición del tipo nodo del nivel 1 */
struct nodeN1{
    stateN0 st; //ubicaciones jugador y colaborador y ult acción colaborador
    list<Action> secuencia;

    bool operator==(const nodeN1 &n) const { //dos nodos son iguales si sus estados son los mismos
        return (st == n.st);
    }

    bool operator<(const nodeN1 &b) const {
        if (st.jugador.f < b.st.jugador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f and st.jugador.c < b.st.jugador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f and st.jugador.c == b.st.jugador.c and 
        st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else if(st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && 
        st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f < b.st.colaborador.f)
            return true;
        else if(st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && 
        st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f == b.st.colaborador.f &&
        st.colaborador.c < b.st.colaborador.c)
            return true;
        else if(st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && 
        st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f == b.st.colaborador.f &&
        b.st.colaborador.c == st.colaborador.c && st.colaborador.brujula < b.st.colaborador.brujula)
          return true;
        else
          return false;
    }
};

struct nodeN2{
    stateN0 st; //ubicaciones jugador y colaborador y ult acción colaborador
    list<Action> secuencia;
    int coste_bateria = 0;
    bool bikini_on = false;
    bool zapatillas_on = false;
    //cada agente tiene capacidad para llevar sólo uno de los dos objetos (zapatillas o bikini)

    bool operator==(const nodeN2 &n) const { //dos nodos son iguales si sus estados son los mismos
        return (st == n.st && bikini_on == n.bikini_on && zapatillas_on == n.zapatillas_on);
    }

    bool operator!=(const nodeN2 &n) const { //dos nodos son iguales si sus estados son los mismos
        return !(st == n.st);
    }

    bool operator<(const nodeN2 &b) const {
        if (coste_bateria < b.coste_bateria) return true;
        else if (coste_bateria == b.coste_bateria && st.jugador.f < b.st.jugador.f)
            return true;
        else if (coste_bateria == b.coste_bateria && st.jugador.f == b.st.jugador.f and st.jugador.c < b.st.jugador.c)
            return true;
        else if (coste_bateria == b.coste_bateria && st.jugador.f == b.st.jugador.f and st.jugador.c == b.st.jugador.c 
        and st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else if (coste_bateria == b.coste_bateria && st.jugador.f == b.st.jugador.f and st.jugador.c == b.st.jugador.c 
        and st.jugador.brujula == b.st.jugador.brujula && bikini_on < b.bikini_on)
            return true;
        else if (coste_bateria == b.coste_bateria && st.jugador.f == b.st.jugador.f and st.jugador.c == b.st.jugador.c 
        and st.jugador.brujula == b.st.jugador.brujula && bikini_on == b.bikini_on && zapatillas_on < b.zapatillas_on)
            return true;
        else
            return false;
    }
    
    
    bool operator>(const nodeN2 &b) const {
        return b < *this; // Invertir la llamada al operador <
    }


    void operator=(const nodeN2 &otro) {
        st = otro.st;
        secuencia = otro.secuencia;
        coste_bateria = otro.coste_bateria;
        bikini_on = otro.bikini_on;
        zapatillas_on = otro.zapatillas_on;
    }
};




class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);


  private:
    // Declarar Variables de Estado
    list<Action> plan; //Almacena el plan en ejecución
    bool hayplan; //Si verdad indica que se está siguiendo un plan.
    stateN0 c_state;
    ubicacion goal;
};

#endif
