#include <boost/random.hpp>

namespace boost{ namespace random{ namespace pyimpl{

struct generators{
    static void instantiate(){
        sizeof( ecuyer1988            );
        sizeof( hellekalek1995        );
        sizeof( kreutzer1986          );
        sizeof( lagged_fibonacci1279  );
        sizeof( lagged_fibonacci19937 );
        sizeof( lagged_fibonacci2281  );
        sizeof( lagged_fibonacci23209 );
        sizeof( lagged_fibonacci3217  );
        sizeof( lagged_fibonacci4423  );
        sizeof( lagged_fibonacci44497 );
        sizeof( lagged_fibonacci607   );
        sizeof( lagged_fibonacci9689  );
        sizeof( minstd_rand           );
        sizeof( minstd_rand0          );
        sizeof( mt11213b              );
        sizeof( mt19937               );
        sizeof( ranlux3               );
        sizeof( ranlux3_01            );
        sizeof( ranlux4               );
        sizeof( ranlux4_01            );
        sizeof( ranlux64_3_01         );
        sizeof( ranlux64_4_01         );
        sizeof( taus88                );
    }
};

#define CREATE_INSTANTIATION( GENERATOR_TYPE )                                                      \
    static result_type                                                                              \
    inline call( distribution& dist, GENERATOR_TYPE& gen ){                                         \
        return dist( gen );                                                                         \
    }                                                                                               \
                                                                                                    \
    typedef variate_generator< GENERATOR_TYPE, distribution > variate_generator_##GENERATOR_TYPE;   \
                                                                                                    \
    variate_generator< GENERATOR_TYPE, distribution >                                               \
    create_vg(distribution dist, GENERATOR_TYPE gen ){                                              \
        return variate_generator< GENERATOR_TYPE, distribution >(gen, dist);                        \
    }
    
struct py_uniform_smallint{
    
    typedef uniform_smallint<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    //CREATE_INSTANTIATION( lagged_fibonacci1279  );
    //CREATE_INSTANTIATION( lagged_fibonacci19937 );
    //CREATE_INSTANTIATION( lagged_fibonacci2281  );
    //CREATE_INSTANTIATION( lagged_fibonacci23209 );
    //CREATE_INSTANTIATION( lagged_fibonacci3217  );
    //CREATE_INSTANTIATION( lagged_fibonacci4423  );
    //CREATE_INSTANTIATION( lagged_fibonacci44497 );
    //CREATE_INSTANTIATION( lagged_fibonacci607   );
    //CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    //CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    //CREATE_INSTANTIATION( ranlux4_01          );
    //CREATE_INSTANTIATION( ranlux64_3_01       );
    //CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_uniform_int{
        
    typedef uniform_int<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    //CREATE_INSTANTIATION( lagged_fibonacci1279  );
    //CREATE_INSTANTIATION( lagged_fibonacci19937 );
    //CREATE_INSTANTIATION( lagged_fibonacci2281  );
    //CREATE_INSTANTIATION( lagged_fibonacci23209 );
    //CREATE_INSTANTIATION( lagged_fibonacci3217  );
    //CREATE_INSTANTIATION( lagged_fibonacci4423  );
    //CREATE_INSTANTIATION( lagged_fibonacci44497 );
    //CREATE_INSTANTIATION( lagged_fibonacci607   );
    //CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    //CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    //CREATE_INSTANTIATION( ranlux4_01          );
    //CREATE_INSTANTIATION( ranlux64_3_01       );
    //CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};
 
 
struct py_uniform_real{
    
    typedef uniform_real<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};
  
struct py_bernoulli_distribution{
    
    typedef bernoulli_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
}; 
 
  
struct py_geometric_distribution{
    
    typedef geometric_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};  
 
struct py_triangle_distribution{
    
    typedef triangle_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
}; 


struct py_exponential_distribution{
    
    typedef exponential_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};


struct py_normal_distribution{
    
    typedef normal_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_uniform_on_sphere{
    
    typedef uniform_on_sphere<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_lognormal_distribution{
    
    typedef lognormal_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_gamma_distribution{
    
    typedef gamma_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_binomial_distribution{
    
    typedef binomial_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_poisson_distribution{
    
    typedef poisson_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

struct py_cauchy_distribution{
    
    typedef cauchy_distribution<> distribution;
    typedef distribution::result_type result_type;

    CREATE_INSTANTIATION( ecuyer1988          );
    CREATE_INSTANTIATION( hellekalek1995      );
    CREATE_INSTANTIATION( kreutzer1986        );
    CREATE_INSTANTIATION( lagged_fibonacci1279  );
    CREATE_INSTANTIATION( lagged_fibonacci19937 );
    CREATE_INSTANTIATION( lagged_fibonacci2281  );
    CREATE_INSTANTIATION( lagged_fibonacci23209 );
    CREATE_INSTANTIATION( lagged_fibonacci3217  );
    CREATE_INSTANTIATION( lagged_fibonacci4423  );
    CREATE_INSTANTIATION( lagged_fibonacci44497 );
    CREATE_INSTANTIATION( lagged_fibonacci607   );
    CREATE_INSTANTIATION( lagged_fibonacci9689  );
    CREATE_INSTANTIATION( minstd_rand         );
    CREATE_INSTANTIATION( minstd_rand0        );
    CREATE_INSTANTIATION( mt11213b            );
    CREATE_INSTANTIATION( mt19937             );
    CREATE_INSTANTIATION( ranlux3             );
    CREATE_INSTANTIATION( ranlux3_01          );
    CREATE_INSTANTIATION( ranlux4             );
    CREATE_INSTANTIATION( ranlux4_01          );
    CREATE_INSTANTIATION( ranlux64_3_01       );
    CREATE_INSTANTIATION( ranlux64_4_01       );
    CREATE_INSTANTIATION( taus88              );
};

} } }


