#include <cilk.h>
#include <memoryweb.h>

typedef long Index_t;

static inline
Index_t n_map(Index_t i) { return i % NODELETS(); }
static inline
Index_t r_map(Index_t i) { return i / NODELETS(); }

static inline
void busy_function(volatile Index_t * p)
{
    Index_t sum = 0;
    Index_t N = 2048;
    for (long i = 0; i < N; ++i) {
         sum += *p;
    }
}

int main(int argc, char* argv[])
{
    starttiming();
    Index_t N = 64;

    Index_t ** a =
        (Index_t **) mw_malloc2d(NODELETS(), (N/NODELETS()) * sizeof(Index_t));
    
    for (Index_t irow = 0; irow < N; ++irow)
    {
        for (Index_t icol = 0; icol < N; ++icol)
        {
            // in the case in mind, icol is same as irow
            Index_t * nodelet_ptr = a[n_map(icol)] + r_map(icol);
            //Index_t * nodelet_ptr = a[n_map(icol)]; // same result
            cilk_migrate_hint(nodelet_ptr);
            cilk_spawn busy_function(nodelet_ptr);
        }
        cilk_sync;
    }

    return 0;
}
