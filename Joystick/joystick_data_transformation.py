#JOYSTICK ESCALADO LINEAL PARAMETRIZABLE CON DIFERENCIAL ASIMÉTRICO
#Resolucion joystick, el rango maximo y el rango minimo de cada eje del joystick
MAX_LECTURA_JOYSTICK_Y = 32767
NEU_LECTURA_JOYSTICK_Y = 0
MIN_LECTURA_JOYSTICK_Y = -32767
NEU_OFFSET_DRIFTING_Y = 1500  # Zona muerta inferior (evita drift cerca de neutro)
MAX_OFFSET_Y = 1500  # Zona de máximo (alcanza máximo antes del límite físico)

MAX_LECTURA_JOYSTICK_X = 32767
NEU_LECTURA_JOYSTICK_X = 0
MIN_LECTURA_JOYSTICK_X = -32767
NEU_OFFSET_DRIFTING_X = 1500  # Zona muerta inferior (evita drift cerca de neutro)
MAX_OFFSET_X = 1500  # Zona de máximo (alcanza máximo antes del límite físico)

#Comportamiento motores - EJE Y (velocidad base)
COMPORTAMIENTO_IZ_MAX_Y = 2000
COMPORTAMIENTO_DR_MAX_Y = 2000
COMPORTAMIENTO_IZ_NEU_Y = 1500
COMPORTAMIENTO_DR_NEU_Y = 1500
COMPORTAMIENTO_IZ_MIN_Y = 1000
COMPORTAMIENTO_DR_MIN_Y = 1000

#Comportamiento motores - EJE X (diferencial asimétrico)
# X positivo (derecha): motor IZ acelera, motor DR mantiene base
# X negativo (izquierda): motor DR acelera, motor IZ mantiene base
DIFERENCIAL_MAX_PWM = 250  # Diferencia máxima entre motores al girar

#Parametros y multiplicadores
MULT_VELOCIDAD = 1 # multiplicador total a la velocidad

# ----- Límite físico global de los motores -----
PWM_MIN = 1000
PWM_MAX = 2000

def clamp_pwm(value):
    """Limita el valor de salida al rango físico global del motor."""
    return max(PWM_MIN, min(PWM_MAX, value))

def normalize_axis(value, dead_zone_min, dead_zone_max, min_val, max_val):
    """Normaliza un eje a rango [-1, 1] aplicando zona muerta inferior y superior."""
    # Aplicar zona muerta cerca del neutro
    if abs(value) < dead_zone_min:
        return 0.0
    
    # Calcular el rango efectivo (restando ambas zonas muertas)
    effective_max = max_val - dead_zone_max
    effective_min = min_val + dead_zone_max
    
    # Normalizar según el lado
    if value > 0:
        if value >= effective_max:
            return 1.0
        # Mapear desde dead_zone_min hasta effective_max como 0..1
        norm = (value - dead_zone_min) / (effective_max - dead_zone_min)
    else:
        if value <= effective_min:
            return -1.0
        # Mapear desde effective_min hasta -dead_zone_min como -1..0
        norm = (value + dead_zone_min) / (effective_min + dead_zone_min)
    
    return max(-1, min(1, norm))

def map_axis(norm, salida_min, salida_neu, salida_max):
    """Mapeo lineal, manteniendo punto neutro."""
    if norm >= 0:
        return salida_neu + norm * (salida_max - salida_neu)
    else:
        return salida_neu + norm * (salida_neu - salida_min)

def calSal(y, x, mul):
    # ----- Normalización con zona muerta inferior y superior -----
    norm_y = normalize_axis(
        y, 
        NEU_OFFSET_DRIFTING_Y,  # Zona muerta inferior (cerca de neutro)
        MAX_OFFSET_Y,            # Zona de máximo (cerca de los extremos)
        MIN_LECTURA_JOYSTICK_Y, 
        MAX_LECTURA_JOYSTICK_Y
    )
    norm_x = normalize_axis(
        x, 
        NEU_OFFSET_DRIFTING_X,  # Zona muerta inferior (cerca de neutro)
        MAX_OFFSET_X,            # Zona de máximo (cerca de los extremos)
        MIN_LECTURA_JOYSTICK_X, 
        MAX_LECTURA_JOYSTICK_X
    )
    
    # ===== EJE Y → VELOCIDAD BASE (ambos motores igual) =====
    base_iz = map_axis(norm_y, COMPORTAMIENTO_IZ_MIN_Y, COMPORTAMIENTO_IZ_NEU_Y, COMPORTAMIENTO_IZ_MAX_Y)
    base_dr = map_axis(norm_y, COMPORTAMIENTO_DR_MIN_Y, COMPORTAMIENTO_DR_NEU_Y, COMPORTAMIENTO_DR_MAX_Y)
    
    # ===== EJE X → DIFERENCIAL ASIMÉTRICO =====
    # norm_x: -1 (izquierda) a +1 (derecha)
    # X > 0 (giro derecha): motor IZ acelera, motor DR se mantiene
    # X < 0 (giro izquierda): motor DR acelera, motor IZ se mantiene
    
    if norm_x > 0:  # Giro a la derecha
        diferencial = norm_x * DIFERENCIAL_MAX_PWM
        vel_iz = base_iz + diferencial
        vel_dr = base_dr  # Se mantiene en velocidad base
    elif norm_x < 0:  # Giro a la izquierda
        diferencial = abs(norm_x) * DIFERENCIAL_MAX_PWM
        vel_iz = base_iz  # Se mantiene en velocidad base
        vel_dr = base_dr + diferencial
    else:  # Sin giro
        vel_iz = base_iz
        vel_dr = base_dr
        diferencial = 0
    
    # ----- Aplicar multiplicador de velocidad desde el punto neutro -----
    # El multiplicador escala la diferencia respecto al neutro, no el valor absoluto
    vel_iz = COMPORTAMIENTO_IZ_NEU_Y + (vel_iz - COMPORTAMIENTO_IZ_NEU_Y) * mul
    vel_dr = COMPORTAMIENTO_DR_NEU_Y + (vel_dr - COMPORTAMIENTO_DR_NEU_Y) * mul
    
    # ----- Ajuste para mantener diferencial cuando se exceden límites -----
    # Si algún motor se pasa del límite, reducimos ambos proporcionalmente
    if vel_iz > PWM_MAX:
        exceso = vel_iz - PWM_MAX
        vel_iz = PWM_MAX
        vel_dr = vel_dr - exceso
    elif vel_dr > PWM_MAX:
        exceso = vel_dr - PWM_MAX
        vel_dr = PWM_MAX
        vel_iz = vel_iz - exceso
    elif vel_iz < PWM_MIN:
        defecto = PWM_MIN - vel_iz
        vel_iz = PWM_MIN
        vel_dr = vel_dr + defecto
    elif vel_dr < PWM_MIN:
        defecto = PWM_MIN - vel_dr
        vel_dr = PWM_MIN
        vel_iz = vel_iz + defecto
    
    # ----- Límite global PWM final -----
    vel_iz = clamp_pwm(vel_iz)
    vel_dr = clamp_pwm(vel_dr)
    
    # ----- Verbosidad clara -----
    print(f"norm_y: {norm_y:.2f}   norm_x: {norm_x:.2f}")
    print(f"base_iz: {base_iz:.0f}   base_dr: {base_dr:.0f}")
    if norm_x > 0:
        print(f"diferencial derecha: +{diferencial:.0f} (IZ acelera)")
    elif norm_x < 0:
        print(f"diferencial izquierda: +{diferencial:.0f} (DR acelera)")
    else:
        print(f"diferencial: 0")
    print(f"vel_iz:  {vel_iz:.0f}   vel_dr: {vel_dr:.0f}")
    print(f"diferencia real: {abs(vel_iz - vel_dr):.0f}")
    print("---")
    
    return vel_iz, vel_dr

# TESTS
print("=== TEST 1: Neutro + Giro Derecha Máximo (X=+1) ===")
print("Esperado: iz=1750, dr=1500")
calSal(0, 32767, 1)

print("\n=== TEST 2: Adelante Máximo + Giro Derecha Máximo ===")
print("Esperado: iz=2000, dr=1750")
calSal(32767, 32767, 1)

print("\n=== TEST 3: Atrás Máximo + Giro Izquierda Máximo ===")
print("Esperado: iz=1000, dr=1250")
calSal(-32767, -32767, 1)

print("\n=== TEST 4: Adelante Máximo sin giro ===")
print("Esperado: iz=2000, dr=2000")
calSal(32767, 0, 1)

print("\n=== TEST 5: Neutro + Giro Izquierda Máximo ===")
print("Esperado: iz=1500, dr=1750")
calSal(0, -32767, 1)

print("\n=== TEST 6: Adelante Medio + Giro Derecha Medio ===")
calSal(16383, 16383, 1)

print("\n=== TEST 7: Adelante casi al máximo (31267) + sin giro ===")
print("Debería alcanzar máximo (2000) con el offset")
calSal(31267, 0, 1)

print("\n=== TEST 8: X casi al máximo (31267) + Y neutro ===")
print("Debería alcanzar diferencial máximo (250)")
calSal(0, 31267, 1)

print("\n=== TEST 9: Adelante Máximo con multiplicador 0.5 ===")
print("Esperado: iz=1500, dr=1500 (mitad de velocidad)")
calSal(32767, 0, 0.5)

print("\n=== TEST 10: Adelante Máximo + Giro Derecha con multiplicador 0.5 ===")
print("Esperado: iz=1500, dr=1375 (mitad, manteniendo diferencial)")
calSal(32767, 32767, 0.5)

print("\n=== TEST 11: Adelante Medio + Giro Derecha con multiplicador 0.3 ===")
print("Esperado: velocidades reducidas al 30%")
calSal(16383, 16383, 0.3)

print("\n=== TEST 12: Neutro + Giro Derecha con multiplicador 0.7 ===")
print("Esperado: iz=1525, dr=1425 (70% del diferencial)")
calSal(0, 32767, 0.7)