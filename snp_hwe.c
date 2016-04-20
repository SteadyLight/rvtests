// obtained from http://www.sph.umich.edu/csg/abecasis/Exact/snp_hwe.c
// 2012-11-29 Xiaowei
/*
// This code implements an exact SNP test of Hardy-Weinberg Equilibrium as described in
// Wigginton, JE, Cutler, DJ, and Abecasis, GR (2005) A Note on Exact Tests of
// Hardy-Weinberg Equilibrium. American Journal of Human Genetics. 76: 000 - 000
//
// Written by Jan Wigginton
*/

/**
 * NOTE (by zhanxw)
 * !! Makesure not all parameters equal to 0, or the program will crash.
 */
inline double SNPHWE(int obs_hets, int obs_hom1, int obs_hom2)
{
  if (obs_hom1 < 0 || obs_hom2 < 0 || obs_hets < 0)
  {
    printf("FATAL ERROR - SNP-HWE: Current genotype configuration (%d  %d %d ) includes a"
           " negative count", obs_hets, obs_hom1, obs_hom2);
    exit(EXIT_FAILURE);
  }

  int obs_homc = obs_hom1 < obs_hom2 ? obs_hom2 : obs_hom1;
  int obs_homr = obs_hom1 < obs_hom2 ? obs_hom1 : obs_hom2;

  int rare_copies = 2 * obs_homr + obs_hets;
  int genotypes   = obs_hets + obs_homc + obs_homr;

  double * het_probs = (double *) malloc((size_t) (rare_copies + 1) * sizeof(double));
  if (het_probs == NULL)
  {
    printf("FATAL ERROR - SNP-HWE: Unable to allocate array for heterozygote probabilities" );
    exit(EXIT_FAILURE);
  }

  int i;
  for (i = 0; i <= rare_copies; i++)
    het_probs[i] = 0.0;

  /* start at midpoint */
  int mid = 1.0 * rare_copies * (2 * genotypes - rare_copies) / (2 * genotypes);

  /* check to ensure that midpoint and rare alleles have same parity */
  if ((rare_copies & 1) ^ (mid & 1))
    mid++;

  int curr_hets = mid;
  int curr_homr = (rare_copies - mid) / 2;
  int curr_homc = genotypes - curr_hets - curr_homr;

  het_probs[mid] = 1.0;
  double sum = het_probs[mid];
  for (curr_hets = mid; curr_hets > 1; curr_hets -= 2)
  {
    het_probs[curr_hets - 2] = het_probs[curr_hets] * curr_hets * (curr_hets - 1.0)
                               / (4.0 * (curr_homr + 1.0) * (curr_homc + 1.0));
    sum += het_probs[curr_hets - 2];

    /* 2 fewer heterozygotes for next iteration -> add one rare, one common homozygote */
    curr_homr++;
    curr_homc++;
  }

  curr_hets = mid;
  curr_homr = (rare_copies - mid) / 2;
  curr_homc = genotypes - curr_hets - curr_homr;
  for (curr_hets = mid; curr_hets <= rare_copies - 2; curr_hets += 2)
  {
    het_probs[curr_hets + 2] = het_probs[curr_hets] * 4.0 * curr_homr * curr_homc
                               /((curr_hets + 2.0) * (curr_hets + 1.0));
    sum += het_probs[curr_hets + 2];

    /* add 2 heterozygotes for next iteration -> subtract one rare, one common homozygote */
    curr_homr--;
    curr_homc--;
  }

  for (i = 0; i <= rare_copies; i++)
    het_probs[i] /= sum;

  /* alternate p-value calculation for p_hi/p_lo
     double p_hi = het_probs[obs_hets];
     for (i = obs_hets + 1; i <= rare_copies; i++)
     p_hi += het_probs[i];

     double p_lo = het_probs[obs_hets];
     for (i = obs_hets - 1; i >= 0; i--)
     p_lo += het_probs[i];


     double p_hi_lo = p_hi < p_lo ? 2.0 * p_hi : 2.0 * p_lo;
  */

  double p_hwe = 0.0;
  /*  p-value calculation for p_hwe  */
  for (i = 0; i <= rare_copies; i++)
  {
    if (het_probs[i] > het_probs[obs_hets])
      continue;
    p_hwe += het_probs[i];
  }

  p_hwe = p_hwe > 1.0 ? 1.0 : p_hwe;

  free(het_probs);

  return p_hwe;
}
