
const char* memo(byte b)
{
	switch(b)
	{
		case 0x00:
			return "NOP";
		break;


		case 0xC3:
			return "JP NZ, a16";
		break;

		default:
			return "????";
		break;
	}
}
