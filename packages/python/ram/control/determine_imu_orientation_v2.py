def read_log_files(filename):
    """
      >>> read_log_files('test.txt')
      [2.0, 3.0, 2.0, 5.0, 5.0, 5.0]
    """
    infile = open(filename, 'r')
    numvals = 0
    accxsum = 0.0
    accysum = 0.0
    acczsum = 0.0
    magxsum = 0.0
    magysum = 0.0
    magzsum = 0.0
    not_finished = True
    while not_finished:
        text = infile.readline()
        #find the imu values
        temp = text.split(' ')
        #acc = temp[6:9]
        #mag = temp[9:12]
        #average
        if len(temp) > 11:
            numvals = numvals + 1
            accx = float(temp[6])#acc[0])
            accy = float(temp[7])#acc[1])
            accz = float(temp[8])#acc[2])
            magx = float(temp[9])#mag[0])
            magy = float(temp[10])#mag[1])
            magz = float(temp[11])#mag[2])
            accxsum = accxsum + accx
            accysum = accysum + accy
            acczsum = acczsum + accz
            magxsum = magxsum + magx
            magysum = magysum + magy
            magzsum = magzsum + magz
        else:
            not_finished = False
    accxavg = accxsum / numvals
    accyavg = accysum / numvals
    acczavg = acczsum / numvals
    magxavg = magxsum / numvals
    magyavg = magysum / numvals
    magzavg = magzsum / numvals
    infile.close()
    return [accxavg, accyavg, acczavg, magxavg, magyavg, magzavg]


def mag(x, y, z):
    """Determines the magnitude of a vector."""                         
    return (x**2 + y**2 + z**2)**0.5

def cross(x1, y1, z1, x2, y2, z2):
    """Calculates the cross product of two vectors and normalizes the result."""
    a = (y1*z2)-(z1*y2)    #i component 
    b = (z1*x2)-(x1*z2)    #j component
    c = (x1*y2)-(y1*x2)    #k component
    m = mag(a, b, c)
    return [a/m, b/m, c/m]    #The norm of the cross product
    
    
def rotation(g1, g2, g3, m1, m2, m3):
    """
      Calculates the rotation matrix R=bCn given the vectors g and m. 

      >>> tol = 0.0001
      >>> R = rotation(0, 0, 1, 0.1813, 0, 0.0845)
      >>> R[0]-1<tol, R[1]-0<tol, R[2]-0<tol
      (True, True, True)
      >>> R[3]-0<tol, R[4]+1<tol, R[5]-0<tol
      (True, True, True)
      >>> R[6]-0<tol, R[7]-0<tol, R[8]+1<tol
      (True, True, True)

      >>> B = rotation(1, 0, 0, 0.0845, -0.1813, 0)
      >>> B[0]-0<tol, B[1]-0<tol, B[2]+1<tol
      (True, True, True)
      >>> B[3]+1<tol, B[4]-0<tol, B[5]-0<tol
      (True, True, True)
      >>> B[6]-0<tol, B[7]-1<tol, B[8]-0<tol
      (True, True, True)

      >>> C = rotation(0, 0, -1, 0.1538, -0.0961, -0.0845)
      >>> C[0]-0.8481<tol, C[1]-0.5299<tol, C[2]-0<tol
      (True, True, True)
      >>> C[3]+0.5299<tol, C[4]-0.8481<tol, C[5]-0<tol
      (True, True, True)
      >>> C[6]-0<tol, C[7]-0<tol, C[8]-1<tol
      (True, True, True)

      >>> D = rotation(0.4226, 0, -0.9063, 0.2, 0, 0)
      >>> D[0]-0.9063<tol, D[1]-0<tol, D[2]+0.4226<tol
      (True, True, True)
      >>> D[3]-0<tol, D[4]-1<tol, D[5]-0<tol
      (True, True, True)
      >>> D[6]-0.4226<tol, D[7]-0<tol, D[8]-0.9063<tol
      (True, True, True)

      >>> E = rotation(0, 0.7071, -0.7071, 0.1813, 0.0598, -0.0598)
      >>> E[0]-1<tol, E[1]-0<tol, E[2]-0<tol
      (True, True, True)
      >>> E[3]-0<tol, E[4]-0.7071<tol, E[5]+0.7071<tol
      (True, True, True)
      >>> E[6]-0<tol, E[7]-0.7071<tol, E[8]-0.7071<tol
      (True, True, True)

      >>> F = rotation(1, 0, 0, 0.0845, 0, 0.1813)
      >>> F[0]-0<tol, F[1]-0<tol, F[2]+1<tol
      (True, True, True)
      >>> F[3]-0<tol, F[4]-1<tol, F[5]-0<tol
      (True, True, True)
      >>> F[6]-1<tol, F[7]-0<tol, F[8]-0<tol
      (True, True, True)
      
      >>> G = rotation(0, -0.7071, -0.7071, 0.1282, -0.1504, 0.0309)
      >>> G[0]-0.7071<tol, G[1]-0.7071<tol, G[2]-0<tol
      (True, True, True)
      >>> G[3]+0.5<tol, G[4]-0.5<tol, G[5]-0.7071<tol
      (True, True, True)
      >>> G[6]-0.5<tol, G[7]+0.5<tol, G[8]-0.7071<tol
      (True, True, True)
    """


    n3i = -g1    #Vector n3 is the opposite of g
    n3j = -g2
    n3k = -g3
    n3mag = mag(n3i, n3j, n3k)    #Magnitude of vector n3
    n3 = [n3i/n3mag, n3j/n3mag, n3k/n3mag]
    #Normalizes n3 and stores as a list        
    n2 = cross(m1, m2, m3, g1, g2, g3)
    #n2 is normalized cross product of m and g, stored as list
    n1 = cross(n2[0], n2[1], n2[2], n3[0], n3[1], n3[2])
    #n1 is normalized cross of n2 and n3, stored as list
    matrix = [n1, n2, n3]
    #Stores n1, n2, and n3 as a list of lists
    R = [n1[0], n2[0], n3[0], n1[1], n2[1], n3[1], n1[2], n2[2], n3[2]]
    return R


def rotation_avg(log_list):
    list = read_log_files(log_list)
    return rotation(list[0], list[1], list[2], list[3], list[4], list[5])
    
    
if __name__ == '__main__':
    import doctest
    doctest.testmod()
    
