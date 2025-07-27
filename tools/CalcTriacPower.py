import math

T = 20.0
U_max = 230*math.sqrt(2)
U_eff_net = 230

def calcRelativePower(t0):
    U_eff_sq = U_max*U_max * (.5 - t0/T + math.sin(4*math.pi * t0/T) / (4*math.pi))
    return U_eff_sq / (U_eff_net*U_eff_net)


t=0
while t <= T/2:
    relPower = calcRelativePower(t)
    print(f"t0={t:.3f}ms -> {relPower*100:.2f}%")
    t+= 0.1