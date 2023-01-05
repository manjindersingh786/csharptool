
#include	"TSIM.h"

//====================================================
//	Transit from Big Endian WORD to Little Endian WORD
//	Transit from Little Endian WORD to Big Endian WORD
void	TransitBigLittle(BYTE * buffer)
{
	int	i;
	BYTE	temp[4];
	
	//	temp[i] = buffer[i],i=0~3,no matter Big or Little Endian
	*(WORD *)temp = *(WORD *)buffer;
	
	for(i=0;i<4;i++)
	{
		buffer[i] = temp[3-i];
	}
}

//====================================================
//	Extract big endian halfword
HALFW	ExtractBEHalfword(BYTE * buffer)
{
	return	((HALFW)(buffer[0] << 8) + buffer[1]);
}

//====================================================
//	Copy memory from forward
void	mem_cpyFwd(BYTE * dst,const BYTE * src,USHORT len)
{
	USHORT	i;
	
	for(i=0;i<len;i++)
	{
		dst[i] = *src++;
	}
}

//====================================================
//	Copy memory from backward
void	mem_cpyBwd(BYTE * dst,const BYTE * src,USHORT len)
{	
	while(len>0)
	{
		dst[len-1] = src[len-1];
		len--;
	}
}

//====================================================
//	Compare memory
BYTE	mem_cmp(const BYTE * dst, const BYTE * src,USHORT len)
{
	USHORT	i;
	
	for(i=0;i<len;i++)
	{
		if(dst[i] != *src++)
			return 1;
	}
	return 0;
}

//====================================================
//	Copy memory auto adapt
void	mem_cpy_autoadapt(BYTE * dst,const BYTE * src,HALFW len)
{
	while(len > 0)
	{
		if(((WORD)src % 4 == 0) && ((WORD)dst % 4 == 0) && (len >= 4))
		{
			*(WORD *)dst = *(WORD *)src;
			dst += 4;
			src += 4;
			len -= 4;
		}
		else if(((WORD)src % 2 == 0) && ((WORD)dst % 2 == 0) && (len >= 2))
		{
			*(HALFW *)dst = *(HALFW *)src;
			dst += 2;
			src += 2;
			len -= 2;
		}
		else
		{
			*dst ++ = *src ++;
			len --;
		}
	}
}
//====================================================
// Set memory with given value
void	mem_set(BYTE * dst,BYTE value,USHORT len){
	USHORT	i;
	
	for(i=0;i<len;i++)
	{
		dst[i] = value;
	}
}
//====================================================
//	Flash area
BYTE	IsFlash(WORD addr)
{
	BYTE rtn;
	if (addr == FlashStart)
	{
		rtn = 1;
	}
	else if (addr > FlashStart)
	{
		if (addr < FlashLimit)
		{
			rtn = 1;
		}
		else
		{
			rtn = 0;
		}
	}
	else
	{
		rtn = 0;
	}
	return rtn;
}

//====================================================
//	OTP area			
BYTE	IsOTP(WORD addr)
{
	return(addr >= OTPStart ? (addr < OTPLimit ? 1 : 0) : 0);
}



