LRESULT rp_Bft(
    LPCOMPLEXSCALAR     ret,
    LPCMCSTRING       fluid,
    LPCCOMPLEXSCALAR      t   )
{
    char herr[255];
    int kph = 1;       //  kph = 1 (saturated liquid conc.); kph = 2 (saturated vapor conc.)  
    int ierr;
    double psat,tsat,rhol,rhov,xliq[20],xvap[20];
    double xkappa, beta = 0.0, xisenk, xkt, betas, bs, xkkt, thrott, pi, spht;

    ierr = cSetup(fluid->str);
    if (ierr > 0)
        return MAKELRESULT(ierr,1);
    
    if( t->imag != 0.0 )
        return MAKELRESULT(MUST_BE_REAL,2);
    else
        tsat = t->real;

    SATTdll(&tsat,&x[0],&kph,&psat,&rhol,&rhov,&xliq[0],&xvap[0],&ierr,herr,errormessagelength);

    if (ierr > 0)
    {
        if ((ierr == 1)||(ierr == 9)||(ierr == 121)) 
            return MAKELRESULT(T_OUT_OF_RANGE,2); // Temperature too low | negative | > Tcrit
        else
            return MAKELRESULT(UNCONVERGED,2); // failed to converge
    }

    THERM3dll(&tsat, &rhol, &x[0], &xkappa,
        &beta,                                // Volume expansion coefficient (= 1/rho dD/dT) [1/K]
        &xisenk, &xkt, &betas, &bs, &xkkt,
        &thrott, &pi, &spht);

    ret->real = beta;       // Return thermal expansion coefficient [1/K]

    return 0;               // return 0 to indicate there was no error
            
}    

FUNCTIONINFO    rp_betaft = 
{ 
    (char *)("rp_betaft"),              // Name by which mathcad will recognize the function
    (char *)("fluid,t"),                // rp_betaft will be called as rp_betaft(fluid,t)
    (char *)("Returns the saturation liquid volume expansion coefficient [1/K] given the temperature [K]"),
                                        // description of rp_betaft(fluid,t)
    (LPCFUNCTION)rp_Bft,                // pointer to the executable code
    COMPLEX_SCALAR,                     // the return type is a complex scalar
    2,                                  // the function takes 2 arguments
    { MC_STRING,                        // argument is a MC_STRING
      COMPLEX_SCALAR }                  // argument is a complex scalar
};
    