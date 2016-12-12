// predict.cc
// This file contains the main function.  The program accepts a single 
// parameter: the name of a trace file.  It drives the branch predictor
// simulation by reading the trace file and feeding the traces one at a time
// to the branch predictor.

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // in case you want to use e.g. memset
#include <assert.h>

#include "branch.h"
#include "trace.h"
#include "predictor.h"
#include "my_predictor.h"

int main (int argc, char *argv[])
{
   // make sure there are at least two parameters

   if (argc != 3 && argc != 4) {
      fprintf(stderr, "Usage: %s <filename>.bz2 table_bits [history_length]\n", argv[0]);
      exit(1);
   }

   // open the trace file for reading

   init_trace(argv[1]);

   // initialize competitor's branch prediction code
   unsigned int n = atoi(argv[2]);
   if (n == 0) {
      fprintf(stderr, "Erreur: nombre de bits definissant la taille de la table à 0 !\n");
      exit(1);
   }
   unsigned int l = 0;

   if (argc == 4) {
      l = atoi(argv[3]);
      if (l == 0) {
         fprintf(stderr, "Erreur: taille de l'historique à zéro !\n");
         exit(1);
      }
   }

   branch_predictor *p = new my_predictor(n, l);

   // some statistics to keep, currently just for conditional branches

   long long int 
      tmiss = 0,  // number of target mispredictions
      dmiss = 0;  // number of direction mispredictions

   // keep looping until end of file

   for (;;) {

      // get a trace

      trace *t = read_trace ();

      // NULL means end of file

      if (!t) break;

      // send this trace to the competitor's code for prediction

      branch_update *u = p->predict (t->bi);

      // collect statistics for a conditional branch trace

      if (t->bi.br_flags & BR_CONDITIONAL) {

         // count a direction misprediction

         dmiss += u->direction_prediction () != t->taken;

         // count a target misprediction

         tmiss += u->target_prediction () != t->target;
      }

      // update competitor's state

      p->update (u, t->taken);
   }

   // done reading traces

   end_trace ();

   // give final mispredictions per kilo-instruction and exit.
   // each trace represents exactly 100 million instructions.

   printf ("%0.3f MPKI\n", 1000.0 * (dmiss / 1e8));
   delete p;
   exit (0);
}
