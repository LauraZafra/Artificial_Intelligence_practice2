#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

/** Devuelve si una ubicación en el mapa es transitable para el agente*/ 
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char> > &mapa){
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M' );
}

//Devuelve la ubicacion siguiente segun el avance del agente
ubicacion NextCasilla(const ubicacion &pos){
	ubicacion next = pos;
	switch (pos.brujula){
		case norte:
		next.f = pos.f - 1;
		break;
		case noreste:
		next.f = pos.f - 1;
		next.c = pos.c + 1;
		break;
		case este:
		next.c = pos.c + 1;
		break;
		case sureste:
		next.f = pos.f + 1;
		next.c = pos.c + 1;
		break;
		case sur:
		next.f = pos.f + 1;
		break;
		case suroeste:
		next.f = pos.f + 1;
		next.c = pos.c - 1;
		break;
		case oeste:
		next.c = pos.c - 1;
		break;
		case noroeste:
		next.f = pos.f - 1;
		next.c = pos.c - 1;
		break;
	}
	return next;
}

/** Devuelve el estado que se genera si el agente puede avanzar.
 * Si no puede avanzar, devuelve como salida el mismo estado de entrada
*/
stateN0 Apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char> > mapa){
	stateN0 st_result = st; //contiene la ubicación del jugador y del colaborador
	ubicacion sig_ubicacion, sig_ubicacion2;
	switch (a)
	{
	case actWALK: //si prox casilla es transitable y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				st_result.jugador = sig_ubicacion;
			} //si no la ubicación se queda como estaba
		break;

    case act_CLB_WALK: //si prox casilla es transitable y no está ocupada por el jugador
		sig_ubicacion = NextCasilla(st.colaborador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c)){
				st_result.colaborador = sig_ubicacion;
			}
		break;
	
	case actRUN: //si prox 2 casillas son transitables y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				sig_ubicacion2 = NextCasilla(sig_ubicacion);
				if (CasillaTransitable(sig_ubicacion2, mapa) and 
					!(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c)){
						st_result.jugador = sig_ubicacion2;
				}
			}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		break;

    case act_CLB_TURN_SR:
		st_result.colaborador.brujula = static_cast<Orientacion>((st_result.colaborador.brujula+1)%8);
		break;
	}

	return st_result;
}


/**Encuentra si el elemento item está en la lista */
bool Find(const stateN0 &item, const list<stateN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() and !((*it) == item))
		it++;

	return (!(it == lista.end()));
}

/**Sobrecarga de la función Find para buscar en lista de nodeN0*/
bool Find(const stateN0 &item, const list<nodeN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() and !(it->st == item))
		it++;

	return (!(it == lista.end()));
}


//Busqueda en anchura
/*
bool AnchuraSoloJugador (const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa){
  stateN0 current_state = inicio;
  list<stateN0> frontier;
  list<stateN0> explored;
  frontier.push_back(current_state);
  bool solution_found = (current_state.jugador.f == final.f && current_state.jugador.c == final.c);

  while (!frontier.empty() && !solution_found){
	frontier.pop_front(); //quito el nodo actual de los no explorados
	explored.push_back(current_state); //y lo meto en los explorados

	// Generar hijo actwALK
	stateN0 child_walk = Apply(actWALK, current_state, mapa);
	if (child_walk.jugador.f == final.f && child_walk.jugador.c == final.c){
		current_state = child_walk;
		solution_found = true;
	}
	//no está explorado ni en la lista de pendientes
	else if (!Find(child_walk, frontier) and !Find (child_walk, explored)){
		frontier.push_back(child_walk);
	}

	if (!solution_found){ //si lo de antes no era sol
		// Generar hijo actRUN
		stateN0 child_run = Apply(actRUN, current_state, mapa); //devuelve resultado de aplicar acción
		if (child_run.jugador.f == final.f && child_run.jugador.c == final.c){
			current_state = child_run;
			solution_found = true;
		}
		//no está explorado ni en la lista de pendientes
		else if (!Find(child_run, frontier) and !Find (child_run, explored)){
			frontier.push_back(child_run);
		}
	}
	
	if (!solution_found){ //si lo de antes no era sol
		// Generar hijo actTURN_L
		stateN0 child_turnl = Apply(actTURN_L, current_state, mapa);
		if (!Find(child_turnl, frontier) and !Find (child_turnl, explored)){
			frontier.push_back(child_turnl);
		}
		// Generar hijo actTURN_SR
		stateN0 child_turnsr = Apply(actTURN_SR, current_state, mapa);
		if (!Find(child_turnsr, frontier) and !Find (child_turnsr, explored)){
			frontier.push_back(child_turnsr);
		}
	}

	//Si no tengo sol pero me quedan nodos por explorar exploro el primero
	if (!solution_found && !frontier.empty()) current_state = frontier.front();	
  }
  return solution_found;
}
*/

/** pone a cero todos los elementos de una matriz */
void AnulaMatriz(vector<vector<unsigned char>> &matriz){
    for (int i = 0; i < matriz.size(); i++)
        for (int j = 0; j < matriz[0].size(); j++)
            matriz[i][j] = 0;
}

/** Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st*/
void VisualizaPlan(const stateN0 &st, const list<Action> &plan, vector<vector<unsigned char>> &matriz){
    AnulaMatriz(matriz);
    stateN0 cst = st;
    auto it = plan.begin();
    while (it != plan.end()){
        if ((*it != act_CLB_WALK) && (*it != act_CLB_TURN_SR) && (*it != act_CLB_STOP)){
            switch (cst.ultimaOrdenColaborador){
                case act_CLB_WALK:
                    cst.colaborador = NextCasilla(cst.colaborador);
                    matriz[cst.colaborador.f][cst.colaborador.c] = 2;
                    break;
                case act_CLB_TURN_SR:
                    cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
                    break;
            }
        }
        switch (*it){
            case actRUN:
                cst.jugador = NextCasilla(cst.jugador);
                matriz[cst.jugador.f][cst.jugador.c] = 3;
                cst.jugador = NextCasilla(cst.jugador);
                matriz[cst.jugador.f][cst.jugador.c] = 1;
                break;
            case actWALK:
                cst.jugador = NextCasilla(cst.jugador);
                matriz[cst.jugador.f][cst.jugador.c] = 1;
                break;
            case actTURN_SR:
                cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 1) % 8);
                break;
            case actTURN_L:
                cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
                break;
            case act_CLB_WALK:
                cst.colaborador = NextCasilla(cst.colaborador);
                cst.ultimaOrdenColaborador = act_CLB_WALK;
                matriz[cst.colaborador.f][cst.colaborador.c] = 2;
                break;
            case act_CLB_TURN_SR:
                cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
                cst.ultimaOrdenColaborador = act_CLB_TURN_SR;
                break;
            case act_CLB_STOP:
                cst.ultimaOrdenColaborador = act_CLB_STOP;
                break;
        }
        it++;
    }
}

void PintaPlan(const list<Action> &plan){
    auto it = plan.begin();
    while (it != plan.end()){
        if (*it == actWALK){
            cout << "W ";
        }
        else if (*it == actRUN){
            cout << "R ";
        }
        else if (*it == actTURN_SR){
            cout << "r ";
        }
        else if (*it == actTURN_L){
            cout << "L ";
        }
        else if (*it == act_CLB_WALK){
            cout << "cW ";
        }
        else if (*it == act_CLB_TURN_SR){
            cout << "cr ";
        }
        else if (*it == act_CLB_STOP){
            cout << "cS ";
        }
        else if (*it == actIDLE){
            cout << "I ";
        }
        else{
            cout << "- ";
        }
        it++;
    }
    cout << " (" << plan.size() << " acciones)\n";
}

/**Versión segunda de la búsqueda en anchura: ahora sí devuelve un plan*/
/*
list<Action> AnchuraSoloJugador_V2(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa){
    nodeN0 current_node;
    list<nodeN0> frontier;
    list<nodeN0> explored;
    list<Action> plan;
    current_node.st = inicio;
    bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
    frontier.push_back(current_node);

    while (!frontier.empty() and !SolutionFound){
        frontier.pop_front();
        explored.push_back(current_node);

        // Generar hijo actWALK
        nodeN0 child_walk = current_node;
        child_walk.st = Apply(actWALK, current_node.st, mapa);
        child_walk.secuencia.push_back(actWALK);
        if (child_walk.st.jugador.f == final.f and child_walk.st.jugador.c == final.c){
			current_node = child_walk;
            SolutionFound = true;
        }
        else if (!Find(child_walk.st, frontier) and !Find(child_walk.st, explored)){
            frontier.push_back(child_walk);
        }

        // Generar hijo actRUN
        if (!SolutionFound){
            nodeN0 child_run = current_node;
            child_run.st = Apply(actRUN, current_node.st, mapa);
            child_run.secuencia.push_back(actRUN);
            if (child_run.st.jugador.f == final.f and child_run.st.jugador.c == final.c){
                current_node = child_run;
				SolutionFound = true;
            }
            else if (!Find(child_run.st, frontier) and !Find(child_run.st, explored)){
                frontier.push_back(child_run);
            }
        }

        // Generar hijos actTURN_L y actTURN_SR
        if (!SolutionFound){
            // Generar hijo actTURN_L
            nodeN0 child_turnL = current_node;
            child_turnL.st = Apply(actTURN_L, current_node.st, mapa);
            child_turnL.secuencia.push_back(actTURN_L);
            if (!Find(child_turnL.st, frontier) and !Find(child_turnL.st, explored)){
                frontier.push_back(child_turnL);
            }

            // Generar hijo actTURN_SR
            nodeN0 child_turnSR = current_node;
            child_turnSR.st = Apply(actTURN_SR, current_node.st, mapa);
            child_turnSR.secuencia.push_back(actTURN_SR);
            if (!Find(child_turnSR.st, frontier) and !Find(child_turnSR.st, explored)){
                frontier.push_back(child_turnSR);
            }
        }

        if (!SolutionFound and !frontier.empty()){
            current_node = frontier.front();
        }
    }
	
    if (SolutionFound){
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(current_node.secuencia);
    }
	
    return plan;
}
*/

list<Action> AnchuraSoloJugador_V3(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa){
    nodeN0 current_node;
    list<nodeN0> frontier;
    set<nodeN0> explored;
    list<Action> plan;
    current_node.st = inicio;
    bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
    frontier.push_back(current_node);

    while (!frontier.empty() and !SolutionFound){
        frontier.pop_front();
        explored.insert(current_node);

        // Generar hijo actWALK
        nodeN0 child_walk = current_node;
        child_walk.st = Apply(actWALK, current_node.st, mapa);
        child_walk.secuencia.push_back(actWALK);
        if (child_walk.st.jugador.f == final.f and child_walk.st.jugador.c == final.c){
			current_node = child_walk;
            SolutionFound = true;
        }
        else if (explored.find(child_walk) == explored.end()){
            frontier.push_back(child_walk);
        }

        // Generar hijo actRUN
        if (!SolutionFound){
            nodeN0 child_run = current_node;
            child_run.st = Apply(actRUN, current_node.st, mapa);
            child_run.secuencia.push_back(actRUN);
            if (child_run.st.jugador.f == final.f and child_run.st.jugador.c == final.c){
                current_node = child_run;
				SolutionFound = true;
            }
            else if (explored.find(child_run) == explored.end()){
                frontier.push_back(child_run);
            }
        }

        // Generar hijos actTURN_L y actTURN_SR
        if (!SolutionFound){
            // Generar hijo actTURN_L
            nodeN0 child_turnL = current_node;
            child_turnL.st = Apply(actTURN_L, current_node.st, mapa);
            child_turnL.secuencia.push_back(actTURN_L);
            if (explored.find(child_turnL) == explored.end()){
                frontier.push_back(child_turnL);
            }

            // Generar hijo actTURN_SR
            nodeN0 child_turnSR = current_node;
            child_turnSR.st = Apply(actTURN_SR, current_node.st, mapa);
            child_turnSR.secuencia.push_back(actTURN_SR);
            if (explored.find(child_turnSR) == explored.end()){
                frontier.push_back(child_turnSR);
            }
        }

        if (!SolutionFound and !frontier.empty()){ //no he encontrado sol pero me quedan nodos por explorar
            current_node = frontier.front();
            while (!frontier.empty() and explored.find(current_node) != explored.end()){ //me quedo con un nodo sin explorar
                frontier.pop_front();
                if (!frontier.empty()) current_node = frontier.front();
            }
        }
    }
	
    if (SolutionFound){
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(current_node.secuencia);
    }
	
    return plan;
}

//-------------------------------------------------------------------------------------------------------------------------
//
//                                                           NIVEL 1
//
//-------------------------------------------------------------------------------------------------------------------------

/*
Implementación del algoritmo en anchura para resolver el problema de planificar un camino para llevar al agente colaborador 
a la casilla destino y seguiremos usando el modelo de ejecución de las acciones anterior pero ahora con las acciones 
asociadas al agente colaborador.

El objetivo del agente jugador es crear y llevar a cabo un plan de acciones en el mapa para hacer llegar al agente 
colaborador desde su posición inicial al destino usando el menor número de acciones.
*/

//El concepto de estado entre los niveles 0 y 1 es distinto. Mientras que en el primero el estado está formado solo por 
//la ubicación del agente jugador, en este segundo caso hay que incluir la ubicación del agente colaborador

bool JugadorVeColaborador (ubicacion jugador, ubicacion colaborador) {
    switch (jugador.brujula){
        case norte:
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            break;
        case sur:
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            break;
        case este:
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            break;
        case oeste:
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            break;
        case noreste:
            if ((jugador.f - 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            break;
        case noroeste:
            if ((jugador.f) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 1) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 2) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f - 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            break;
        case sureste:
            if ((jugador.f) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 3) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c + 1) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            break;
        case suroeste:
            if ((jugador.f + 1) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 1) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 2) == colaborador.c) return true;
            if ((jugador.f + 3) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f + 2) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f + 1) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            if ((jugador.f) == colaborador.f && (jugador.c - 3) == colaborador.c) return true;
            break;
    }
    return false;
}

bool ColaboradorEnFinal (const nodeN1 &current_node, const ubicacion &final){
    //cout << "colaborador fila " << current_node.st.colaborador.f << endl;
    //cout << "colaborador col " << current_node.st.colaborador.c << endl;
    return (current_node.st.colaborador.f == final.f and current_node.st.colaborador.c == final.c);
}

bool ComprobarFinal (const nodeN1 child, nodeN1 &current_node, const ubicacion &final){
    if (ColaboradorEnFinal(child, final)){
        //child_clb_walk.secuencia.push_back(act_CLB_STOP);
        cout << "colaborador fila " << child.st.colaborador.f << endl;
        cout << "colaborador col " << child.st.colaborador.c << endl;
        cout << final.f << endl;
        current_node = child;
        return true;
    }
}



list<Action> AnchuraColaborador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, const vector<unsigned char> &agentes){
    nodeN1 current_node;
    list<nodeN1> frontier;
    set<nodeN1> explored;
    list<Action> plan;
    current_node.st = inicio;
    //ahora la solución se ha alcanzado si el colaborador ha alcanzado el objetivo (final)
    bool SolutionFound = ColaboradorEnFinal(current_node, final);
    frontier.push_back(current_node);
    
    while (!frontier.empty() and !SolutionFound){
        //Comienzo a explorar siguiente nodo
        frontier.pop_front();
        explored.insert(current_node);
       
        if (!SolutionFound && JugadorVeColaborador(current_node.st.jugador, current_node.st.colaborador)){ //si no veo al colaborador no le puedo dar órdenes

            // Generar hijo act_CLB_WALK
            nodeN1 child_clb_walk = current_node;
            child_clb_walk.st = Apply(act_CLB_WALK, current_node.st, mapa);
            //if (child_clb_walk.st.ultimaOrdenColaborador != act_CLB_WALK) 
            child_clb_walk.secuencia.push_back(act_CLB_WALK);
            child_clb_walk.st.ultimaOrdenColaborador = act_CLB_WALK;
            
            SolutionFound = ComprobarFinal(child_clb_walk, current_node, final);
            
            if (!SolutionFound && explored.find(child_clb_walk) == explored.end()){ //si el nodo no se ha explorado lo exploro
                frontier.push_back(child_clb_walk);
            }

            // Generar hijo act_CLB_TURN_SR
            if (!SolutionFound){
                nodeN1 child_clb_turnSR = current_node;
                child_clb_turnSR.st = Apply(act_CLB_TURN_SR, current_node.st, mapa);
                //if (child_clb_turnSR.st.ultimaOrdenColaborador != act_CLB_TURN_SR) 
                child_clb_turnSR.secuencia.push_back(act_CLB_TURN_SR);
                child_clb_walk.st.ultimaOrdenColaborador = act_CLB_TURN_SR;
                
                if (explored.find(child_clb_turnSR) == explored.end()){ //si el nodo no se ha explorado lo exploro
                    frontier.push_back(child_clb_turnSR);
                }
            }

            // Generar hijo act_CLB_STOP
            /*
            if (!SolutionFound){
                nodeN1 child_clb_stop = current_node;
                child_clb_stop.secuencia.push_back(act_CLB_STOP);
                if (explored.find(child_clb_stop) == explored.end()){ //si el nodo no se ha explorado lo exploro
                    frontier.push_back(child_clb_stop);
                }
            } 
            */ 
            
        }

        //si no veo al colaborador y no tengo sol
        if (!SolutionFound){
            // Generar hijo actWALK
            nodeN1 child_walk = current_node;
            child_walk.st = Apply(actWALK, current_node.st, mapa);
            child_walk.st = Apply(child_walk.st.ultimaOrdenColaborador, child_walk.st, mapa);
            
            SolutionFound = ComprobarFinal(child_walk, current_node, final);
            
            if (!SolutionFound && explored.find(child_walk) == explored.end()){ //si no lo he explorado lo exploro
                child_walk.secuencia.push_back(actWALK);
                frontier.push_back(child_walk);
            }
        }

        if (!SolutionFound){
            // Generar hijo actRUN
            nodeN1 child_run = current_node;
            child_run.st = Apply(actRUN, current_node.st, mapa);
            child_run.st = Apply(child_run.st.ultimaOrdenColaborador, child_run.st, mapa);
            
            SolutionFound = ComprobarFinal(child_run, current_node, final);
            
            if (!SolutionFound && explored.find(child_run) == explored.end()){
                child_run.secuencia.push_back(actRUN);
                frontier.push_back(child_run);
            }
        }
            
        if (!SolutionFound){
            // Generar hijo actTURN_L
            nodeN1 child_turnL = current_node;
            child_turnL.st = Apply(actTURN_L, current_node.st, mapa);
            child_turnL.st = Apply(child_turnL.st.ultimaOrdenColaborador, child_turnL.st, mapa);
            
            SolutionFound = ComprobarFinal(child_turnL, current_node, final);
            
            if (!SolutionFound && explored.find(child_turnL) == explored.end()){
                child_turnL.secuencia.push_back(actTURN_L);
                frontier.push_back(child_turnL);   
            }
        }
        
        if (!SolutionFound){
            // Generar hijo actTURN_SR
            nodeN1 child_turnSR = current_node;
            child_turnSR.st = Apply(actTURN_SR, current_node.st, mapa);
            child_turnSR.st = Apply(child_turnSR.st.ultimaOrdenColaborador, child_turnSR.st, mapa);

            SolutionFound = ComprobarFinal(child_turnSR, current_node, final);
            
            if (!SolutionFound && explored.find(child_turnSR) == explored.end()){
                child_turnSR.secuencia.push_back(actTURN_SR);
                frontier.push_back(child_turnSR);
            }
        }
        

        // Generar hijo actIDLE
        nodeN1 child_idle = current_node;
        child_idle.st = Apply(child_idle.st.ultimaOrdenColaborador, child_idle.st, mapa);
        if (explored.find(child_idle) == explored.end()){ //si no lo he explorado lo exploro
            child_idle.secuencia.push_back(actIDLE);
            frontier.push_back(child_idle);
        }
        

        //Busco sig nodo a explorar
        if (!SolutionFound and !frontier.empty()){ //no he encontrado sol pero me quedan nodos por explorar
            current_node = frontier.front();
            while (!frontier.empty() and explored.find(current_node) != explored.end()){ //me quedo con un nodo sin explorar
                frontier.pop_front();
                if (!frontier.empty()) current_node = frontier.front();
            }
        }      
    }

    cout << "Tam de frontier al final: " << frontier.size() << endl;
    cout << "Tam de explored: " << explored.size() << endl;

    if (SolutionFound){
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(current_node.secuencia);
    }
	
    cout << "Tam de plan: " << plan.size() << endl;
    return plan;
    //se generan nodos diferentes si el jugador esta en diferentes posiciones
}


//-------------------------------------------------------------------------------------------------------------------------
//
//                                                           NIVEL 2
//
//-------------------------------------------------------------------------------------------------------------------------
/*
Encontrar el camino con el mínimo consumo de batería usando el ALGORITMO DE DIJKSTRA (coste uniforme) que lleve al 
agente JUGADOR a una casilla objetivo

Ahora el agente jugador debe encontrar la secuencia de acciones que le permita llegar a una casilla del mapa.
Las diferencias con el nivel anterior son: el tipo de camino que se pide (el mínimo en consumo de batería) y 
que el plan solo puede contener acciones para el agente jugador.

El objetivo es construir y ejecutar con éxito un plan (con solo acciones para el agente jugador) para llevar 
al agente jugador desde su posición inicial al destino con el menor consumo de batería posible 
(usando el algoritmo de Dijsktra).
*/

bool JugadorEnFinal(const ubicacion &jugador, const ubicacion &final){
    return (jugador.f == final.f && jugador.c == final.c);
}

int CosteN2(Action accion, char inicio, bool bikini_on, bool zapatillas_on){
    switch (accion){
        case actIDLE: return 0;
        case act_CLB_STOP: return 0;
        case actWHEREIS: return 200;
        case actWALK:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 10;
                    else return 100;
                case 'B':
                    if (zapatillas_on) return 15;
                    else return 50;
                case 'T': return 2;
                default: return 1;
            }
        case act_CLB_WALK:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 10;
                    else return 100;
                case 'B':
                    if (zapatillas_on) return 15;
                    else return 50;
                case 'T': return 2;
                default: return 1;
            }
        case actRUN:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 15;
                    else return 150;
                case 'B':
                    if (zapatillas_on) return 25;
                    else return 75;
                case 'T': return 3;
                default: return 1;
            }
        case actTURN_L:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 5;
                    else return 30;
                case 'B':
                    if (zapatillas_on) return 1;
                    else return 7;
                case 'T': return 2;
                default: return 1;
            }
        case actTURN_SR:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 2;
                    else return 10;
                case 'B':
                    if (zapatillas_on) return 1;
                    else return 5;
                case 'T': return 1;
                default: return 1;
            }
        case act_CLB_TURN_SR:
            switch (inicio){
                case 'A':
                    if (bikini_on) return 2;
                    else return 10;
                case 'B':
                    if (zapatillas_on) return 1;
                    else return 5;
                case 'T': return 1;
                default: return 1;
            }
    }
    return 0;
}

priority_queue<nodeN2> Copia (priority_queue<nodeN2> otro){
    priority_queue<nodeN2> sol;
    nodeN2 aux;
    while (!otro.empty()){
        aux = otro.top();
        otro.pop();
        sol.push(aux);
    }
    return sol;
}

bool FindPriority(const nodeN2 &nodo, const priority_queue<nodeN2> original){
    priority_queue<nodeN2> copia = Copia(original);
    nodeN2 aux;
    while (!copia.empty()){
        aux = copia.top();
        if (aux == nodo) return true;
        copia.pop();
    }
    return false;
}

bool MenorCoste (int a, int b){
    return (a < b);
}

priority_queue<nodeN2> NuevaFrontier (const nodeN2 &nuevo, const priority_queue<nodeN2> original) {
    priority_queue<nodeN2> copia = Copia(original);
    priority_queue<nodeN2> sol;
    nodeN2 aux;
    while (!copia.empty()) {
        aux = copia.top();
        copia.pop();
        if (aux.st != nuevo.st || !MenorCoste(nuevo.coste_bateria, aux.coste_bateria)){ 
            sol.push(aux);   
        } 
        else if (aux.st == nuevo.st && MenorCoste(nuevo.coste_bateria, aux.coste_bateria)){ //si ambos nodos tienen el mismo estado intercambio si el nuevo tiene menor coste
            sol.push(nuevo);
        }
    }
    return sol;
}

void CogerObjeto (nodeN2 &nodo, char destino){
    if (destino == 'K'){
        nodo.bikini_on = true;
        nodo.zapatillas_on = false;
    }
    else if (destino == 'D'){
        nodo.bikini_on = false;
        nodo.zapatillas_on = true;
    }
}


list<Action> UniformeDijkstra (const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, const vector<unsigned char> &agentes){
    nodeN2 current_node; //PATH-COST = 0
    priority_queue<nodeN2> frontier; //priority queue ordered by PATH-COST (coste de batería), with node as the only element
    set<nodeN2> explored; //empty
    list<Action> plan;
   
    current_node.st = inicio;
    frontier.push(current_node);
    char ini;


    while (!frontier.empty()){  
        current_node = frontier.top(); //chooses the lowest-cost node in frontier  
        frontier.pop();
        
        if (JugadorEnFinal(current_node.st.jugador, final)){
            plan = current_node.secuencia;
            PintaPlan(plan);
            cout << "Coste total: " << current_node.coste_bateria << endl;
            return plan; //he encontrado sol
        } 
        
        explored.insert(current_node);

        // Generar hijo actWALK
        nodeN2 child_walk = current_node;
        ini = mapa[child_walk.st.jugador.f][child_walk.st.jugador.c];
        child_walk.st = Apply(actWALK, current_node.st, mapa);
        child_walk.secuencia.push_back(actWALK);
        child_walk.coste_bateria = child_walk.coste_bateria + CosteN2(actWALK, ini, child_walk.bikini_on, child_walk.zapatillas_on);
        CogerObjeto(child_walk, mapa[child_walk.st.jugador.f][child_walk.st.jugador.c]); //cojo el objeto antes de avanzar a la casilla

        bool esta_en_frontier_walk = FindPriority(child_walk, frontier);        if (esta_en_frontier_walk) cout << "ESTA";
        if (explored.find(child_walk) == explored.end() && !esta_en_frontier_walk){ //no esta en ninguno
			frontier.push(child_walk); //insertar en orden: menor coste al frente
        }
        else if (esta_en_frontier_walk){ //está en frontier con mayor coste --> reemplazar  
            priority_queue<nodeN2> frontier_nueva = NuevaFrontier(child_walk, frontier);  
            frontier.swap(frontier_nueva); //swap reemplaza una cola por otra
        }
        

        
        // Generar hijo actRUN
        nodeN2 child_run = current_node;
        ini = mapa[child_run.st.jugador.f][child_run.st.jugador.c];
        child_run.st = Apply(actRUN, current_node.st, mapa);
        child_run.secuencia.push_back(actRUN);
        child_run.coste_bateria = child_run.coste_bateria + CosteN2(actRUN, ini, child_run.bikini_on, child_run.zapatillas_on);
        CogerObjeto(child_run, mapa[child_run.st.jugador.f][child_run.st.jugador.c]);
        
        bool esta_en_frontier_run = FindPriority(child_run, frontier);
        if (explored.find(child_run) == explored.end() && !esta_en_frontier_run){ //no esta en ninguno
			frontier.push(child_run); //insertar en orden: menor coste al frente
        }
        else if (esta_en_frontier_run){ //está en frontier con mayor coste --> reemplazar  
            priority_queue<nodeN2> frontier_nueva = NuevaFrontier(child_run, frontier);  
            frontier.swap(frontier_nueva); //swap reemplaza una cola por otra
        }
        
        

        // Generar hijo actTURN_L
        nodeN2 child_turnL = current_node;
        ini = mapa[child_turnL.st.jugador.f][child_turnL.st.jugador.c];
        child_turnL.st = Apply(actTURN_L, current_node.st, mapa);
        child_turnL.secuencia.push_back(actTURN_L);
        child_turnL.coste_bateria = child_turnL.coste_bateria + CosteN2(actTURN_L, ini, child_turnL.bikini_on, child_turnL.zapatillas_on);
        //cout << "Coste giro izq: " << child_turnL.coste_bateria << endl;
        bool esta_en_frontier_turnl = FindPriority(child_turnL, frontier);
        if (explored.find(child_turnL) == explored.end() && !esta_en_frontier_turnl){ //no esta en ninguno
			frontier.push(child_turnL); //insertar en orden: menor coste al frente
        }
        else if (esta_en_frontier_turnl){ //está en frontier con mayor coste --> reemplazar  
            priority_queue<nodeN2> frontier_nueva = NuevaFrontier(child_turnL, frontier);  
            frontier.swap(frontier_nueva); //swap reemplaza una cola por otra
        }
        
        

        // Generar hijo actTURN_SR
        nodeN2 child_turnSR = current_node;
        ini = mapa[child_turnSR.st.jugador.f][child_turnSR.st.jugador.c];
        child_turnSR.st = Apply(actTURN_SR, current_node.st, mapa);
        child_turnSR.secuencia.push_back(actTURN_SR);
        child_turnSR.coste_bateria = child_turnSR.coste_bateria + CosteN2(actTURN_SR, ini, child_turnSR.bikini_on, child_turnSR.zapatillas_on);
        
        bool esta_en_frontier_turnsr = FindPriority(child_turnSR, frontier);
        if (explored.find(child_turnSR) == explored.end() && !esta_en_frontier_turnsr){ //no esta en ninguno
			frontier.push(child_turnSR); //insertar en orden: menor coste al frente
        }
        else if (esta_en_frontier_turnsr){ //está en frontier con mayor coste --> reemplazar  
            priority_queue<nodeN2> frontier_nueva = NuevaFrontier(child_turnSR, frontier);  
            frontier.swap(frontier_nueva); //swap reemplaza una cola por otra
        }
        cout << "Tam frontier " << frontier.size() << endl;
        
    }
    

    return plan;
}










//-------------------------------------------------------------------------------------------------------------------------
//
//                                                           MÉTODO PRINCIPAL
//
//-------------------------------------------------------------------------------------------------------------------------

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
    if (sensores.nivel != 4){
        if (!hayplan){
            c_state.jugador.f = sensores.posF;
            c_state.jugador.c = sensores.posC;
            c_state.jugador.brujula = sensores.sentido;
            c_state.colaborador.f = sensores.CLBposF;
            c_state.colaborador.c = sensores.CLBposC;
            c_state.colaborador.brujula = sensores.CLBsentido;
            goal.f = sensores.destinoF;
            goal.c = sensores.destinoC;
            cout << "Calculando un nuevo plan..." << endl;
            switch (sensores.nivel){
                case 0: 
                    plan = AnchuraSoloJugador_V3(c_state, goal, mapaResultado);
                    break;
                case 1: 
                    //Elaborar un plan para hacer que el colaborador llegue al objetivo
                    plan = AnchuraColaborador(c_state, goal, mapaResultado, sensores.agentes);
                    break;
                case 2: // Incluir aquí la llamada al alg. búsqueda del nivel 2
                    plan = UniformeDijkstra(c_state, goal, mapaResultado, sensores.agentes);
                    break;
                case 3: // Incluir aquí la llamada al alg. búsqueda del nivel 3
                    cout << "Pendiente de implementar el nivel 3\n";
                    break;
            }

            if (plan.size()>0){
                VisualizaPlan(c_state, plan, mapaConPlan);
                hayplan = true;
            }    
        }
        //Ejecutar el plan
        if (hayplan and plan.size()>0){
            accion = plan.front();
            plan.pop_front();
        }
        if (plan.size() == 0){
            cout << "Plan completado" << endl;
            hayplan = false;
        }
    }
    else{
        //implementación N4
    }
	return accion;
}


int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
