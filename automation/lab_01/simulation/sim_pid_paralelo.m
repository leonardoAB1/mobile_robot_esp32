close all
clear
clc
%Datos
y=zeros(1,600);
u=zeros(1,600);
e=zeros(1,600);

fs=10;
Ts=1/fs;
%Parametros de la planta
A=0.08067;
B=0.9832;
%Parametros del controlador
kp=0.000000000008;
kd=0.000000001;
ki=0.05;
a=2*kp*Ts;
b=ki*Ts^2;
c=4*kd;
d=2*Ts;
umax=1.5;
umin=0;
%Referencia
yr(1:200)=70;
yr(201:400)=50;
yr(401:600)=90;

%Simulación
N=600;
for k=5:N
    %ecuación de la planta
    y(k) =A*u(k-1)+B*y(k-1)+2.3;
    %calculo del error
    e(k)=yr(k)-y(k);
    %controlador
    u(k)=((a+b+c)/d)*e(k)+((2*(b-c))/d)*e(k-1)...
        +((b+c-a)/d)*e(k-2)+u(k-2);
    %u(k)=1;
    
    if u(k) > umax
       u(k) = umax;
    elseif u(k) < umin
       u(k) = umin;
    end
    
end

t=1:N;

subplot(3,1,1)
plot(t, y(t), "k", "linewidth", 2), hold on
plot(t, yr(t),"--r","linewidth",2)
legend("y(k)", "yr(k)"), grid
title("Señal de referencia")

subplot(3,1,2)
plot(t,e(t),"k","linewidth",2)
legend("e(k)"), grid
title("Error")
xlabel("Muestra")

subplot(3,1,3)
plot(t,u(t),"k","linewidth",2)
legend("u(k)"), grid
title("Control")
xlabel("Muestra")