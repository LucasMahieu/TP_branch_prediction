// Pedicteur 2bits à 4 états sans histoique
#include <inttypes.h>
//
// Ajout d'une information à la class branch_update à titre d'exemple
class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
	my_update u;
	branch_info bi;
	uint8_t *table;
   unsigned int table_bits;

   // Constructeur
   // 2^table_bits entrées de 2 bits
	my_predictor(unsigned int pcbits, unsigned int histlen)
   { 
      // Alloue et met à zéro la table
      table = new uint8_t [1<<pcbits]();
      table_bits = pcbits;
      // Pas d'utilisation de l'historique global pour ce prédicteur
	}

   // Calcul de la prédiction
	branch_update *predict(branch_info &b)
   {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
         // Saut conditionnel
         // Récupération des bits de l'adresse pour indexer la table
			u.index = (b.address & ((1<<table_bits)-1));
         // Choix de la direction (la mise à jour se fait dans update
			u.direction_prediction(next_predict(get_state(u.index)));
		} else {
         // Saut inconditionnel, 100% sur que c'est pris !
			u.direction_prediction(true);
		}
		return &u;
	}

	// donne une prediction en fonction de l'état de la case i du tableau
	bool next_predict(unsigned int i) 
	{
		  switch (i) {
					 case 0:
					 // 0b00
								return false;
					 case 1:
					 // 0b01
								return false;
					 case 2:
					 // 0b10
								return true;
					 case 3:
					 // 0b11
								return true;
					 default :
								exit(1);
		  }
	}

	// lig = ligne de la table 
	// pos = colonne de la table, 1 ou 2
	unsigned int get_state(int lig){
			  return table[lig] & 3;
	}
	// lig = ligne de la table 
	// met à jour l'état de la fsm
	void set_state(int lig, bool e){
	unsigned int i = get_state(lig);
		switch (i) {
					 case 0:
					 // 0b00 : SNT
								table[lig] = e ? 1 : 0;
								break;
					 case 1:
					 // 0b01 : NT
								table[lig] = e ? 3 : 0;
								break;
					 case 2:
								table[lig] = e ? 3 : 0;
								break;
					 case 3:
								table[lig] = e ? 3 : 2;
								break;
		  } 
	}

   // Mise à jour de la table de prédiction
	void update(branch_update *u, bool taken) {
      // Saut conditionnel
      // On peut forcer à true ou false pour avoir les extrêmes
		if (bi.br_flags & BR_CONDITIONAL) {
			//table[((my_update*)u)->index] = taken;
			set_state(((my_update*)u)->index, taken);
		}
	}
};
// vim:se ts=3:
