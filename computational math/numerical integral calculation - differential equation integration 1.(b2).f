c     ===================================
c     MAIN PROGRAM
c		This program takes a function and
c		computes its definite integral in
c		a region (a, b)
c     ===================================
      program main

c	  Variables
      real*8 a, b, x1, x2
      real*8 f
      a = 0.0d0
      b = 2.0d0*datan(1.0d0)
      x1 = -1.0d0
      x2 = 1.0d0

      res = f(x1,a,b) + f(x2,a,b)
      write(*,*) 'result:', res
      
      end



c     =========================================
c	  FUNCTION
c		Definition of the function to integrate
c     =========================================
      real*8 function f(x,a,b)
      real*8 x, a, b
      
      f = ((b-a)/2.0d0)*log( -1.0d0 + ((b+a)/2.0d0) + x*(b-a)/2.0d0 )
      
      return
      end