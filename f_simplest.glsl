#version 330 //obliczenia per pixel

uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 n; //wektor normalny
in vec4 l1; //wektor do zrodla swiatla
in vec4 l2;
in vec4 v; //
in vec2 iTexCoord0;

void main(void)
{
	vec4 vn = normalize(v);
    vec4 nn = normalize(n);
    vec4 ln1 = normalize(l1);
    vec4 l2n = normalize(l2);

	vec4 r = reflect(-ln1,n); //odbite l, kat padania = katowi odbicia
    vec4 r2 = reflect(-l2n,n); //negujemy bo potrzebujemy wektor od swiatla, a mamy do

	vec4 kd = texture(textureMap0, iTexCoord0); //kolor materialu dla rozproszonego
	vec4 ka = texture(textureMap0, iTexCoord0); //dla otoczenia
	vec4 ks = texture(textureMap0, iTexCoord0); //dla odbitego
	
	vec4 la = vec4(0.3,0.3,0.3,1); //kolor swiatla otoczenia
    vec4 ld = vec4(0.2,0.2,0.15,1); //rozpraszanego
	vec4 ls = vec4(1, 1, 1,0); //odbitego
	
	float nl = clamp(dot(nn,ln1), 0, 1); //iloczyn skalarny - wektory do swiatla i normalny - cos kata
    float nl2 = clamp(dot(nn,l2n), 0, 1); 

	float rv = pow(clamp(dot(r,vn), 0, 1), 5); //iloczyn skalarny - wektory do obserwatora i odbity
    float rv2 = pow(clamp(dot(r2,vn), 0, 1), 5); //iloczyn skalarny - wektory do obserwatora i odbity
	
	//im wiekszy wykladnik tym szybciej iloczyn skalarny - cos spada do zera, czyli swiatlo bardziej skupione
	
	pixelColor=(ka*la + kd*ld*vec4(nl,nl,nl,1) + ks*ls*vec4(rv,rv,rv,0)) + (ka*la+kd*ld*vec4(nl2,nl2,nl2,1) + ks*ls*vec4(rv2,rv2,rv2,0));
	//														(r, g, b, 0)
	//phong = (ka*la + kd*ld*(n*l)+ks*ls*(rv))
}