#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;

	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;	

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;
	
	/* 데이터를 저장하기위한 벡터 .*/
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;

	/* 연산을 하기위한 벡터. */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;

	/* 데이터를 저장하기위한 행렬.*/
	typedef		XMFLOAT4X4					_float4x4;

	/* 연산을 하기위한 행렬. */
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

}

#endif // Engine_Typedef_h__
