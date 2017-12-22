c	=========================
c	MAIN PROGRAM
c		4th order Runge Kutta
c	=========================
      program main
      real*8 f, g
      real*8 t, p, th, dt, E, E0, error
      real*8 k1, k2, k3, k4, l1, l2, l3, l4
c		The dimension of the following tables must be equal to the number of iterations N
      real*8 ts(1000), ps(1000), ths(1000), Es(1000)
      integer N, i, j
      i = 1

c	Parameters
      N = 1000

c	Initial values
      E0 = 15.0d-1
      h = 1.0d0
10    continue
      t = 0.0d0
      th = 0.0d0
      p = sqrt(2.0d0+2.0d0*E0)

c	RK4 coefficients
20    continue
      k1 = h*f(t, th, p)
      l1 = h*g(t, th, p)
      k2 = h*f(t+5.0d-1*h, th+5.0d-1*k1, p+5.0d-1*l1)
      l2 = h*g(t+5.0d-1*h, th+5.0d-1*k1, p+5.0d-1*l1)
      k3 = h*f(t+5.0d-1*h, th+5.0d-1*k2, p+5.0d-1*l2)
      l3 = h*g(t+5.0d-1*h, th+5.0d-1*k2, p+5.0d-1*l2)
      k4 = h*f(t+h, th+k1, p+l1)
      l4 = h*g(t+h, th+k1, p+l1)

c	Next value of variables
      t = t + h
      ts(i) = t
      th = th + h*(k1 + 2.0d0*k2 + 2.0d0*k3 + k4)/6.0d0
      ths(i) = th
      p = p + h*(l1 + 2.0d0*l2 + 2.0d0*l3 + l4)/6.0d0
      ps(i) = p

c	Energy and error in Energy
      E = p**2/2.0d0 - dcos(th)
      Es = E
      error = abs((E-E0)/E0)

c	Checking for adequacy in the error
      if (error.gt.1.0d-6) then
        h = h/2.0d0
        i = 1
        go to 10
      endif

c	Continuing with the N iterations
      i = i + 1
      if (i.le.N) then
        go to 20
      endif

c	Export of values for t, p, th, E
      open(unit=51, file='dataT.txt')
      do j=1,N
        write(51,*) ts(j)
      enddo
      close(51)

      open(unit=52, file='dataP.txt')
      do j=1,N
        write(52,*) ps(j)
      enddo
      close(52)

      open(unit=53, file='dataTH.txt')
      do j=1,N
        write(53,*) ths(j)
      enddo
      close(53)

      open(unit=54, file='dataE.txt')
      do j=1,N
        write(54,*) Es(j)
      enddo
      close(54)

c	End of main program
      end



c	==========
c	FUNCTION
c		q' = p
c	==========
      real*8 function f(t,th,p)
      real*8 p, th, t 
      
      f = p
            
      return
      end

c	=================
c	FUNCTION
c		p' = -sin(th)
c	=================
      real*8 function g(t,th,p)
      real*8 th,p,t
      
      g = -dsin(th)
            
      return
      end

