! ------------------------------------------------------
!     _ _       _        _                _
!    | (_)_ __ | | __   | | ___ __   ___ | |_ ___
!    | | | '_ \| |/ /   | |/ / '_ \ / _ \| __/ __|
!    | | | | | |   <    |   <| | | | (_) | |_\__ \
!    |_|_|_| |_|_|\_\___|_|\_\_| |_|\___/ \__|___/
!                  |_____|
! ------------------------------------------------------

SUBROUTINE link_knots(x10, y10, alpha10, x20, y20, alpha20, lenght1, curv1, lenght2, curv2, alpha1add, alpha2add) ! , swap)

    !USE ZBRENT
    !USE ZBREN_INT
    !#Report
    ! Given two knots of coordinates (x,y) and orientation alpha,
    ! join them with two circular arc of length s and curvature c=1/radius
    ! a solution may be found under condition |alpha1-alpha2| < PI
    IMPLICIT NONE
    !declare arguments
    REAL(8), INTENT (IN) :: x10, y10, alpha10 ! coordinates and orientation of the 1st knot
    REAL(8), INTENT (IN) :: x20, y20, alpha20 ! coordinates and orientation of the 2nd knot
    REAL(8), INTENT (OUT) :: lenght1, curv1 ! positive length and curvature of the 1st circular arc
    REAL(8), INTENT (OUT) :: lenght2, curv2 ! positive length and curvature of the 2nd circular arc
    REAL(8), INTENT (OUT) :: alpha1add, alpha2add ! angle added to alpha1 and alpha2
    !LOGICAL, INTENT (OUT):: swap
    !#EndReport
    !declare EXTERNAL
    REAL(8), EXTERNAL :: ARCS_DIFFERENCE, ZBRENT, arcs_50_50,arcs_25_75,arcs_75_25
    !declare common
    REAL(8) alpha1, alpha2, gamma12
    REAL(8) xk1, yk1, ak1, xk2, yk2, ak2
    COMMON /knots/ xk1, yk1, ak1, xk2, yk2, ak2 ! these data are stored for the optimization process
    !declare local
    REAL(8) rho
    REAL(8) L, c1, s1, s2, beta1, beta2, ix1, ix2
    ! Declare UVMIF variables
    INTEGER MAXFN, NOUT
    REAL(8) BOUND, FX, STEP, rho0, beta0
    ! Declare DUVMGS variables
    REAL(8) rhoMAX, rhoMIN, rhoACC,rho_ott, max_curv
    ! EXTERNAL UMACH, DUVMIF
    REAL(8) ERRABS, ERRREL, A, B
    REAL(8) x1, y1, x2, y2, xx0, yy0, xx1, yy1, xx2, yy2
    ! integrate path eqns
    REAL(8) curv0
    COMMON curv0
    !EXTERNAL path_eqns2008
    REAL(8) Y(3), YPRIME(3)
    INTEGER NP, IDO, I, IOUT, IERR, IOUT2
    CHARACTER, PARAMETER :: TAB = CHAR(9)
    REAL, PARAMETER :: PI = 3.14159265

    !***FIRST EXECUTABLE STATEMENT **********************************
    x1 = x10
    y1 = y10
    x2 = x20
    y2 = y20

    !swap = .FALSE.
    !PI = 4.D0*atan(1.D0)

    alpha1 = alpha10
    alpha2 = alpha20

    IF (abs(alpha1 - alpha2) .LT. 1D-4) THEN
        ! special case: the initial and final direction are equal
        beta0 = atan2(y2 - y1, x2 - x1) - alpha1
        if (beta0 .GE. PI) beta0 = beta0 - 2 * PI
        if (beta0 .LE. -PI) beta0 = beta0 + 2 * PI
        L = sqrt((x1 - x2)**2 + (y1 - y2)**2) ! length
        IF (abs(sin(beta0)) < 1.D-5) THEN
            lenght1 = L/2 ! straight line
        ELSE
            lenght1 = abs(L/2 * beta0/sin(beta0))
        ENDIF
        curv1 = 2 * beta0/lenght1
        alpha1add = 0
        ! 2nd circular arc
        curv2 = -curv1
        lenght2 = lenght1
        alpha2add = 0
    ELSE

        MAXFN = 50

        ! controllo che il il versore v1 sia orientato da P1 a P2
        IF ((cos(alpha1)*(x2 - x1) + sin(alpha1)*(y2 - y1)) .GE. 0) THEN
            ! tengo la dx rispetto alla convenzione del nodo k1
            ix1 = +1
        ELSE
            ! tengo la sx rispetto alla convenzione del nodo k1
            ix1 = -1
            alpha1 = alpha1 + PI
        END IF
        ! controllo che il il versore v2 sia orientato da P1 a P2
        IF ((cos(alpha2)*(x2 - x1) + sin(alpha2)*(y2 - y1)) .GE. 0) THEN
            ! tengo la dx rispetto alla convenzione del nodo k2
            ix2 = +1
        ELSE
            ! tengo la sx rispetto alla convenzione del nodo k2
            ix2 = -1
            alpha2 = alpha2 + PI
        END IF

        ! mi riporto nell'intervallo -Pi, Pi
        alpha1 = atan2(sin(alpha1), cos(alpha1))
        alpha2 = atan2(sin(alpha2), cos(alpha2))
        IF (ABS(alpha1 - alpha2) .GT. PI) THEN
            ! la differenza angolare � maggiore di PI, riporto gli angoli nel dominio 0..2Pi
            IF (alpha1 .LT. 0) alpha1 = alpha1 + 2 * PI
            IF (alpha2 .LT. 0) alpha2 = alpha2 + 2 * PI
        ENDIF
        alpha1add = alpha1 - alpha10
        alpha2add = alpha2 - alpha20

        ! store knot data into <arcs_penalty> function
        xk1 = x1
        yk1 = y1
        ak1 = alpha1
        xk2 = x2
        yk2 = y2
        ak2 = alpha2

        GOTO 20
        ! Initialize variables
        10 rho0 = 0.0
        rhoACC = 1D-4 ! accuracy
        BOUND = 1.0
        STEP = 0.2
        ! miminmize the difference of arclenghts (by using golden section search)
        20 A = -3.16
        B = +3.16
        ERRABS = 0.001
        ERRREL = 0.00001

        !CALL ZBREN (arcs_difference, A, B)  ! IMSL IntelFortran10
        rho = ZBRENT(arcs_difference, A, B, 1D-4) ! IntelFortran10
        CALL manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho, lenght1, curv1, lenght2, curv2)

        ! link with arcs of same length
        !rho = ZBRENT(arcs_difference, A, B, 1D-3) ! IntelFortran10
        !rho = ZBRENT(arcs_50_50, A, B, 1D-3)
        !CALL manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho, lenght1, curv1, lenght2, curv2)
        !rho_ott = rho
        !max_curv = MAX(abs(curv1),abs(curv2))

        !rho = ZBRENT(arcs_25_75, A, B, 1D-3)
        !CALL manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho, lenght1, curv1, lenght2, curv2)
        !IF  (max_curv .GT.  MAX(abs(curv1),abs(curv2))) rho_ott = rho   ! choose new solution

        !rho = ZBRENT(arcs_75_25, A, B, 1D-3)
        !CALL manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho, lenght1, curv1, lenght2, curv2)
        !IF  (max_curv .GT.  MAX(abs(curv1),abs(curv2))) rho_ott = rho   ! choose new solution

        ! final interpolation
        !CALL manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho_ott, lenght1, curv1, lenght2, curv2)



    ENDIF

    RETURN
END SUBROUTINE

!                                       _     _ _       _        _                _
!     _ __ ___   __ _ _ __  _   _  __ _| |   | (_)_ __ | | __   | | ___ __   ___ | |_ ___
!    | '_ ` _ \ / _` | '_ \| | | |/ _` | |   | | | '_ \| |/ /   | |/ / '_ \ / _ \| __/ __|
!    | | | | | | (_| | | | | |_| | (_| | |   | | | | | |   <    |   <| | | | (_) | |_\__ \
!    |_| |_| |_|\__,_|_| |_|\__,_|\__,_|_|___|_|_|_| |_|_|\_\___|_|\_\_| |_|\___/ \__|___/
!                                       |_____|            |_____|


SUBROUTINE manual_link_knots(x1, y1, alpha1, x2, y2, alpha2, rho, lenght1, curv1, lenght2, curv2)
    !####    !DEC$ ATTRIBUTES DLLEXPORT::manual_link_knots
    !#Report
    ! Given two knots of coordinates (x,y) and orientation alpha,
    ! they may be connected with two circular arc in many different manner
    ! The indetermination may be resolved by defining the relative angular amplitude of the two arcs,
    ! i.e. by setting    rho = beta1/(beta1+beta2)
    ! where beta1 and beta2 are the angles of circular arcs
    !
    ! NOTES
    !    1) IT MUST BE 0 < |alpha1-alpha2| < PI
    !    2) curvatures may be either positive, negative or null, curvatures may be either concordant or discordant
    !    3) arclenght  may be either >0 or <0, in the 2nd case the congiunction between arcs is a singolarity
    !    4) null arclenght are discarded
    !
    IMPLICIT NONE
    REAL(8), INTENT(IN) :: rho
    !declare arguments
    REAL(8), INTENT (IN) :: x1, y1, alpha1 ! coordinates and orientation of the 1st knot
    REAL(8), INTENT (IN) :: x2, y2, alpha2 ! coordinates and orientation of the 2nd knot
    REAL(8), INTENT (OUT) :: lenght1, curv1 ! positive length and curvature of the 1st circular arc
    REAL(8), INTENT (OUT) :: lenght2, curv2 ! positive length and curvature of the 2nd circular arc
    !#EndReport
    !declare local
    REAL(8) beta1, beta2, ISa1a2, Sa1b1, Ca1b1, b1s, Sa2b2, Ca2b2, b2s, toll
    ! Declare UVMIF variables
    INTEGER MAXFN, NOUT
    REAL(8) BOUND, F, FX, STEP, X, XACC, XGUESS
    ! EXTERNAL UMACH, DUVMIF
    toll = 1e-2

    !***FIRST EXECUTABLE STATEMENT **********************************
    !IF (ABS(alpha1 - alpha2) .LT. 1D-4) THEN
    !    WRITE(*, *) 'ERROR in subroutine <choose_arcs>: argument <alpha2> must be different from  <alpha1>'
    !    lenght1 = 0.0
    !    curv1 = 0.0
    !    lenght2 = 0.0
    !    curv2 = 0.0
    !    RETURN
    !ENDIF

    ! set the angles of arcs
    beta1 = +rho
    beta2 = -rho + alpha2 - alpha1

    ISa1a2 = 1/sin(alpha1/2 - alpha2/2)

    Sa1b1 = sin(alpha1 + beta1/2)
    Ca1b1 = cos(alpha1 + beta1/2)
    IF (abs(beta1) > 1.D-5) THEN
        b1s = beta1/2/sin(beta1/2)
    ELSE
        b1s = 1.0
    ENDIF

    Sa2b2 = sin(alpha2 - beta2/2)
    Ca2b2 = cos(alpha2 - beta2/2)
    IF (abs(beta2) > 1.D-5) THEN
        b2s = beta2/2/sin(beta2/2)
    ELSE
        b2s = 1.0
    ENDIF

    ! 1st circular arc
    lenght1 = ((x1 - x2) * Sa2b2 + (-y1 + y2) * Ca2b2) * b1s * ISa1a2
    IF (ABS(lenght1) < 1e-2) lenght1 = SIGN(toll, lenght1) ! avoid singularities
    curv1 = beta1/lenght1

    ! 2nd circular arc
    lenght2 = ((-x1 + x2) * Sa1b1 + (y1 - y2) * Ca1b1) * b2s * ISa1a2
    IF (ABS(lenght2) < 1e-2) lenght2 = SIGN(toll, lenght2) ! avoid singularities
    curv2 = beta2/lenght2

    RETURN
END SUBROUTINE


!                               _ _  __  __
!      __ _ _ __ ___ ___     __| (_)/ _|/ _| ___ _ __ ___ _ __   ___ ___
!     / _` | '__/ __/ __|   / _` | | |_| |_ / _ \ '__/ _ \ '_ \ / __/ _ \
!    | (_| | | | (__\__ \  | (_| | |  _|  _|  __/ | |  __/ | | | (_|  __/
!     \__,_|_|  \___|___/___\__,_|_|_| |_|  \___|_|  \___|_| |_|\___\___|
!                      |_____|

REAL(8) FUNCTION arcs_difference(rho) RESULT(F)
    IMPLICIT NONE
    !#Report
    REAL(8), INTENT(IN) :: rho
    !#EndReport
    !declare common
    REAL(8) xk1, yk1, ak1, xk2, yk2, ak2
    COMMON /knots/ xk1, yk1, ak1, xk2, yk2, ak2
    !declare local
    REAL(8) lenght1, curv1, lenght2, curv2

    CALL manual_link_knots(xk1, yk1, ak1, xk2, yk2, ak2, rho, lenght1, curv1, lenght2, curv2)
    F = lenght1 - lenght2

    RETURN
END FUNCTION




REAL(8) FUNCTION arcs_50_50(rho) RESULT(F)
    IMPLICIT NONE
    !#Report
    REAL(8), INTENT(IN) :: rho
    !#EndReport
    !declare common
    REAL(8) xk1, yk1, ak1, xk2, yk2, ak2
    COMMON /knots/ xk1, yk1, ak1, xk2, yk2, ak2
    !declare local
    REAL(8) lenght1, curv1, lenght2, curv2

    CALL manual_link_knots(xk1, yk1, ak1, xk2, yk2, ak2, rho, lenght1, curv1, lenght2, curv2)
    F = 0.50*lenght1 - 0.50*lenght2

    RETURN
END FUNCTION




REAL(8) FUNCTION arcs_25_75(rho) RESULT(F)
    IMPLICIT NONE
    !#Report
    REAL(8), INTENT(IN) :: rho
    !#EndReport
    !declare common
    REAL(8) xk1, yk1, ak1, xk2, yk2, ak2
    COMMON /knots/ xk1, yk1, ak1, xk2, yk2, ak2
    !declare local
    REAL(8) lenght1, curv1, lenght2, curv2

    CALL manual_link_knots(xk1, yk1, ak1, xk2, yk2, ak2, rho, lenght1, curv1, lenght2, curv2)
    F = 0.25*lenght1 - 0.75*lenght2

    RETURN
END FUNCTION


REAL(8) FUNCTION arcs_75_25(rho) RESULT(F)
    IMPLICIT NONE
    !#Report
    REAL(8), INTENT(IN) :: rho
    !#EndReport
    !declare common
    REAL(8) xk1, yk1, ak1, xk2, yk2, ak2
    COMMON /knots/ xk1, yk1, ak1, xk2, yk2, ak2
    !declare local
    REAL(8) lenght1, curv1, lenght2, curv2

    CALL manual_link_knots(xk1, yk1, ak1, xk2, yk2, ak2, rho, lenght1, curv1, lenght2, curv2)
    F = 0.75*lenght1 - 0.25*lenght2

    RETURN
END FUNCTION


FUNCTION zbrent(func, x1, x2, tol)
    IMPLICIT NONE
    REAL(8), INTENT(IN) :: x1, x2, tol
    REAL(8) :: zbrent
    INTERFACE
        FUNCTION func(x)
            !USE nrtype
            IMPLICIT NONE
            REAL(8), INTENT(IN) :: x
            REAL(8) :: func
        END FUNCTION func
    END INTERFACE
    INTEGER, PARAMETER :: ITMAX = 100
    REAL(8), PARAMETER :: EPS = epsilon(x1)
    INTEGER :: iter
    REAL(8) :: a, b, c, d, e, fa, fb, fc, p, q, r, s, tol1, xm
    a = x1
    b = x2
    fa = func(a)
    fb = func(b)
    if ((fa > 0.0 .and. fb > 0.0) .or. (fa < 0.0 .and. fb < 0.0)) &
    call nrerror('root must be bracketed for zbrent')
    c = b
    fc = fb
    do iter = 1, ITMAX
        if ((fb > 0.0 .and. fc > 0.0) .or. (fb < 0.0 .and. fc < 0.0)) then
            c = a
            fc = fa
            d = b - a
            e = d
        end if
        if (abs(fc) < abs(fb)) then
            a = b
            b = c
            c = a
            fa = fb
            fb = fc
            fc = fa
        end if
        tol1 = 2.0 * EPS * abs(b) + 0.5 * tol
        xm = 0.5 * (c - b)
        if (abs(xm) <= tol1 .or. fb == 0.0) then
            zbrent = b
            RETURN
        end if
        if (abs(e) >= tol1 .and. abs(fa) > abs(fb)) then
            s = fb/fa
            if (a == c) then
                p = 2.0 * xm * s
                q = 1.0 - s
            else
                q = fa/fc
                r = fb/fc
                p = s * (2.0 * xm * q * (q - r)-(b - a)*(r - 1.0))
                q = (q - 1.0)*(r - 1.0)*(s - 1.0)
            end if
            if (p > 0.0) q = -q
            p = abs(p)
            if (2.0 * p < min(3.0 * xm * q - abs(tol1 * q), abs(e * q))) then
                e = d
                d = p/q
            else
                d = xm
                e = d
            end if
        else
            d = xm
            e = d
        end if
        a = b
        fa = fb
        b = b + merge(d, sign(tol1, xm), abs(d) > tol1)
        fb = func(b)
    end do
    call nrerror('zbrent: exceeded maximum iterations')
    zbrent = b
END FUNCTION zbrent




SUBROUTINE nrerror(string)
    CHARACTER(LEN = *), INTENT(IN) :: string
    write (*, *) 'nrerror: ', string
    STOP 'program terminated by nrerror'
END SUBROUTINE nrerror
