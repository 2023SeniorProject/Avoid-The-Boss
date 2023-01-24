float4 main(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}

//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. 
float Rectangle(float2 f2NDC, float fLeft, float fRight, float fTop, float fBottom)
{
	float2 f2Shape = float2(step(fLeft, f2NDC.x), step(f2NDC.x, fRight));
	f2Shape *= float2(step(fTop, f2NDC.y), step(f2NDC.y, fBottom));
	
	return(f2Shape.x * f2Shape.y);
}
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float2 f2NDC = float2(input.x / 640, input.y / 480) - 0.5f; // (0, 1) : (-0.5, 0.5)
	f2NDC.x *= (640 / 480);
	
	float2 f2Horizontal = float2(0.1f, +0.3f); //(Left, Right)
	float2 f2Vertical = float2(-0.3f, +0.3f); //(Top, Bottom)
	
	cColor.b = Rectangle(f2NDC, +0.1f, +0.3f, -0.2f, +0.4f);
	cColor.b += Rectangle(f2NDC, -0.3f, -0.1f, -0.4f, -0.1f);
	
	return(cColor);
}