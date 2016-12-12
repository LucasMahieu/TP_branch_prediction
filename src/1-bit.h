// Prédicteur naïf 1-bit qui recopie la dernière décision prise

// Ajout d'une information à la class branch_update à titre d'exemple
class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
	my_update u;
	branch_info bi;
	bool *table;
   unsigned int table_bits;

   // Constructeur
   // 2^table_bits entrées de 1 bits
	my_predictor(unsigned int pcbits, unsigned int histlen)
   { 
      // Alloue et met à zéro la table
      table = new bool [1<<pcbits]();
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
			u.direction_prediction(table[u.index]);
		} else {
         // Saut inconditionnel, 100% sur que c'est pris !
			u.direction_prediction(true);
		}
		return &u;
	}

   // Mise à jour de la table de prédiction
	void update(branch_update *u, bool taken) {
      // Saut conditionnel
      // On peut forcer à true ou false pour avoir les extrêmes
		if (bi.br_flags & BR_CONDITIONAL) {
			table[((my_update*)u)->index] = taken;
		}
	}
};
// vim:se ts=3:
