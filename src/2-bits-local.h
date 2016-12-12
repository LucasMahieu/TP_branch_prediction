// Pedicteur 2bits à 4 états sans histoique
#include <inttypes.h>

#define SNT 0b00
#define NT 0b01
#define T 0b10
#define ST 0b11


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
		long *history;
		uint8_t hist_size;

		// Constructeur
		// 2^table_bits entrées de 2 bits
		my_predictor(unsigned int pcbits, unsigned int histlen)
		{
			// Alloue et met à zéro la table
			history = new long[1<<pcbits]();
			table = new uint8_t [1<<pcbits]();

			hist_size = histlen;
			table_bits = pcbits;
		}

		// Calcul de la prédiction
		branch_update *predict(branch_info &b)
		{
			bi = b;
			if (b.br_flags & BR_CONDITIONAL) {
				// Saut conditionnel
				// Récupération des bits de l'adresse pour indexer la table
				//u.index = history & ((1<<hist_size)-1);
				u.index = history[(b.address
							& ((1<<table_bits)-1))];
				// Choix de la direction (la mise à jour se fait dans update)
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
				case SNT:
					// 0b00
					return false;
				case NT:
					// 0b01
					return false;
				case T:
					// 0b10
					return true;
				case ST:
					// 0b11
					return true;
				default :
					return false;
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
				case SNT:
					table[lig] = e ? 1 : 0;
					break;
				case NT:
					table[lig] = e ? 3 : 0;
					break;
				case T:
					table[lig] = e ? 3 : 0;
					break;
				case ST:
					table[lig] = e ? 3 : 2;
					break;
			}
		}

		void update_history(int index, bool taken)
		{
			history[index] = (taken==true) ?
				(history[index]<<1 & ((1<<hist_size)-1)) | 1
				: (history[index]<<1 & ((1<<hist_size)-1)) | 0;
		}

		// Mise à jour de la table de prédiction
		void update(branch_update *u, bool taken) {
			// Saut conditionnel
			// On peut forcer à true ou false pour avoir les extrêmes
			if (bi.br_flags & BR_CONDITIONAL) {
				set_state(((my_update*)u)->index, taken);
				update_history(((my_update*)u)->index,taken);
			}
		}
};
