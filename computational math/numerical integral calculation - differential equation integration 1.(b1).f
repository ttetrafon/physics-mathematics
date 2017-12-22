c     ===================================
c     MAIN PROGRAM
c		This program takes a function and
c		computes its definite integral in
c		a region (a, b)
c     ===================================
      program main

c	  Variables
      real*8 a, b, h, hn, error, conv, res, reslast, sum, x
      real*8 f, df2, df4
      integer n, choice, i, j, l
      j = 1
      conv = 1

c     Parameters
c		defining the region to integrate
      a = 0.0d0
      b = 2.0d0*datan(1.0d0)

c     User's choice of polynomial order
1     write(*,*) 'Choose the order of the polynomial (1, 2, or 3)'
      read(*,*) choice

      if ((choice.ne.1) .and. (choice.NE.2) .and. (choice.NE.3)) then
        write(*,*) '--- not supported choice ---'
        go to 1
      endif
      if (choice .eq. 1) then
        go to 10
      endif
      if (choice .eq. 2) then
        go to 20
      endif
      if (choice .eq. 3) then
        go to 30
      endif
c     end of polynomial choice


c     ---------------------------------
c	  1st degree polynomial - Trapezoid
c     ---------------------------------
10    continue
      write(*,*) 'Method of choice: Trapezoid (1st degree polynomial)'
c	  Starting parameters
      n = 2
      l = 2

15    continue
      res = 0
      sum = 0
      x = a
      hn = h(a,b,n)

c     loop to calculate f0 + 2f1 + 2f2 + ... + 2fn-1 + fn
      i = 1
11    continue
      if (i.gt.1) then
        x = x + hn
      endif
      if (i.eq.1 .or. i.eq.n) then
        sum = sum + f(x)
      else
        sum = sum + 2 * f(x)
      endif
      i = i + 1
c		ending of loop
      if (i.lt.(n+1)) then
        go to 11
      endif

c	  Calculation of the result
      res = sum * (hn/2.0d0)

c     Use of Romberg's method
      if (j.gt.1) then
        reslast = res
        res = res - (res - reslast)/(l**2 - 1)
        conv = reslast - res
      endif

c	  Calculation of the error
      error = - (1.0d0/12.0d0) * hn * (b-a) * df2( (b-a)/2 )

      if (error.gt.1.0d-9 .or. abs(conv).gt.1.0d-9) then
        j = j + 1
        n = n * l
        go to 15
      endif
      go to 99
c	  End of Trapezoid


c     -------------------------------
c	  2nd degree polynomial - Simpson
c     -------------------------------
20    continue
      write(*,*) 'Method of choice: Simpson (2nd degree polynomial)'
c	  Starting parameters
      n = 2
      l = 2

25    continue
      res = 0
      sum = 0
      x = a
      hn = h(a,b,n)

c     loop to calculate f1 + 4f2 + f3 + ... + fn-1 + 4fn + fn+1
      i = 1
21    continue
      if (i.gt.1) then
        x = x + hn
      endif
      if (i.eq.1 .or. i.eq.n) then
        sum = sum + f(x)
      elseif (mod(i,2).eq.0) then
        sum = sum + 4 * f(x)
      else
        sum = sum + 2 * f(x)
      endif
      i = i + 1
c		ending of loop
      if (i.lt.(n+1)) then
        go to 21
      endif

c	  Calculation of the result
      res = sum * (hn/3.0d0)

c     Use of Romberg's method
      if (j.gt.1) then
        reslast = res
        res = res - (res - reslast)/(l**4 - 1)
        conv = reslast - res
      endif

c	  Calculation of the error
      error = - (1.0d0/80.0d0) * (hn**4) * (b-a) * df4( (b-a)/2 )

      if (error.gt.1.0d-9 .or. abs(conv).gt.1.0d-9) then
        j = j + 1
        n = n * l
        go to 25
      endif

      go to 99
c	  End of Simpson


c     -----------------------------------
c	  3rd degree polynomial - Simpson 3/8
c     -----------------------------------
30    continue
      write(*,*) 'Method of choice: Simpson 3/8 (3rd degree polynomial)'
c	  Starting parameters
      n = 3
      l = 2

35    continue
      res = 0
      sum = 0
      x = a
      hn = h(a,b,n)

c     loop to calculate f0 + 2f1 + 2f2 + ... + 2fn-1 + fn
      i = 1
31    continue
      if (i.gt.1) then
        x = x + hn
      endif
      if (i.eq.1 .or. i.eq.n) then
        sum = sum + f(x)
      elseif (mod(i,3).eq.1) then
        sum = sum + 2 * f(x)
      else
        sum = sum + 3 * f(x)
      endif
      i = i + 1
c		ending of loop
      if (i.lt.(n+1)) then
        go to 31
      endif

c	  Calculation of the result
      res = sum * (3.0d0*hn/8.0d0)

c     Use of Romberg's method
      if (j.gt.1) then
        reslast = res
        res = res - (res - reslast)/(l**4 - 1)
        conv = reslast - res
      endif

c	  Calculation of the error
      error = - (1.0d0/12.0d0) * hn * (b-a) * df2( (b-a)/2 )

      if (error.gt.1.0d-9 .or. abs(conv).gt.1.0d-9) then
        j = j + 1
        n = n * l
        go to 35
      endif
      go to 99
c 	  End of Simpson 3/8


99    continue
      write(*,*) '-----------------------------------'
      write(*,*) 'number of points: ', n
      write(*,*) 'result: ', res
      write(*,*) 'error', error
      write(*,*) 'convergence', conv
      end




c     =========================================
c	  FUNCTION
c		Definition of the function to integrate
c     =========================================
      real*8 function f(x)
      real*8 x
      
      f = log(x+1)
      
      return
      end


c     =============================================
c	  FUNCTION
c		Definition of the function's 2nd derivative
c     =============================================
      real*8 function df2(x)
      real*8 x
      
      df2 = - 1.0d0 / (x-1.0d-1)**2
      
      return
      end


c     =============================================
c	  FUNCTION
c		Definition of the function's 4th derivative
c     =============================================
      real*8 function df4(x)
      real*8 x
      
      df4 = - 6.0d0 / (x-1.0d-1)**4
      
      return
      end


c     ==============================================
c	  FUNCTION
c		calculating the steps from the needed points
c     ==============================================
      real*8 function h(a,b,n)
      real*8 a, b
      integer n
      
      h = (b-a)/(n-1)
      
      return
      end

