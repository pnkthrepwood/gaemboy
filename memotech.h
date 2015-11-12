
const char* memo(byte b)
{
	switch(b)
	{
//0x0
		case 0x00:
			return "NOP";
		break;
		case 0x01:
			return "LD BC, d16";
		break;
		case 0x02:
			return "LD (BC), A";
		break;
		case 0x03:
			return "INC BC";
		break;
		case 0x04:
			return "INC B";
		break;
		case 0x05:
			return "DEC B";
		break;
		case 0x06:
			return "LD B, d8";
		break;
		case 0x07:
			return "RLCA";
		break;
		case 0x08:
			return "LD (a16), SP";
		break;
		case 0x09:
			return "ADD HL, BC";
		break;
		case 0x0A:
			return "LD A, (BC)";
		break;
		case 0x0B:
			return "DEC BC";
		break;
		case 0x0C:
			return "INC C";
		break;
		case 0x0D:
			return "DEC C";
		break;
		case 0x0E:
			return "LD C, d8";
		break;
		case 0x0F:
			return "RRCA";
		break;
//0x1
		case 0x10:
			return "STOP 0";
		break;
		case 0x11:
			return "LD DE, d16";
		break;
		case 0x12:
			return "LD (DE), A";
		break;
		case 0x13:
			return "INC DE";
		break;
		case 0x14:
			return "INC D";
		break;
		case 0x15:
			return "DEC D";
		break;
		case 0x16:
			return "LD D, d8";
		break;
		case 0x17:
			return "RLA";
		break;
		case 0x18:
			return "JR r8";
		break;
		case 0x19:
			return "ADD HL, DE";
		break;
		case 0x1A:
			return "LD A, (DE)";
		break;
		case 0x1B:
			return "DEC DE";
		break;
		case 0x1C:
			return "INC E";
		break;
		case 0x1D:
			return "DEC E";
		break;
		case 0x1E:
			return "LD E, d8";
		break;
		case 0x1F:
			return "RRA";
		break;
//0x2
		case 0x20:
			return "JR NZ, r8";
		break;
		case 0x21:
			return "LD HL, d16";
		break;
		case 0x22:
			return "LD (HL+), A";
		break;
		case 0x23:
			return "INC HL";
		break;
		case 0x24:
			return "INC H";
		break;
		case 0x25:
			return "DEC H";
		break;
		case 0x26:
			return "LD H, d8";
		break;
		case 0x27:
			return "DAA";
		break;
		case 0x28:
			return "JR Z, r8";
		break;
		case 0x29:
			return "ADD HL, HL";
		break;
		case 0x2A:
			return "LD A, (HL+)";
		break;
		case 0x2B:
			return "DEC HL";
		break;
		case 0x2C:
			return "INC L";
		break;
		case 0x2D:
			return "DEC L";
		break;
		case 0x2E:
			return "LD L, d8";
		break;
		case 0x2F:
			return "CPL";
		break;



		case 0x3E:
			return "LD A, d8";
		break;

		case 0xAF:
			return "XOR A";
		break;

		case 0xC3:
			return "JP NZ, a16";
		break;

		case 0xE0:
			return "LD [0xFFnn], A";
		break;

		case 0xF0:
			return "LD A, [0xFFnn]";
		break;

		case 0xF3:
			return "DI";
		break;

		default:
			return "????";
		break;
	}
}
