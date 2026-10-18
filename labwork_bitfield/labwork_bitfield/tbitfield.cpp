// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//   Update без сборки для Microsoft Visual Studio 2026 Бусько П.В.
// 
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len)
{
    if (len < 0)
        throw invalid_argument("Imposible length");
    BitLen = len;
    size_t BitInElem = sizeof(TELEM) * 8;
    MemLen = (len + BitInElem - 1) / BitInElem;
    pMem = new TELEM[MemLen];
    memset(pMem, 0, MemLen * sizeof(TELEM));
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
    MemLen = bf.MemLen;
    BitLen = bf.BitLen;
    pMem = new TELEM[MemLen];
    memcpy(pMem, bf.pMem, MemLen * sizeof(TELEM));
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if (n < 0 || n >= BitLen)
        throw invalid_argument("Invalid range");
    return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    return (TELEM)1 << (n % (sizeof(TELEM) * 8));
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || BitLen <= n)
        throw invalid_argument("Bit index out of range");
    pMem[GetMemIndex(n)] |= GetMemMask(n);

}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || BitLen <= n)
        throw invalid_argument("Bit index out of range");
    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || BitLen <= n)
        throw invalid_argument("Bit index out of range");
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) != 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
    TBitField tmp(bf);
    swap(pMem, tmp.pMem);
    swap(BitLen, tmp.BitLen);
    swap(MemLen, tmp.MemLen);
    return (*this);
}

int TBitField::operator==(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen) return 0;
    for (size_t i = 0; i < MemLen; i++) {
        if (pMem[i]!= bf.pMem[i]) {
            return 0;
        } 
    } 
    return 1;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen) return 1;
    for (size_t i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 1;
        } 
    
    }return 0;
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
    size_t MaxLen = max(BitLen, bf.BitLen);
    TBitField tmp(MaxLen);
    if (MemLen >= bf.MemLen) {
        for (size_t i = 0; i < bf.MemLen; i++) {
            tmp.pMem[i] = pMem[i] | bf.pMem[i];
        }
        for (size_t i = bf.MemLen; i < MemLen; i++) {
            tmp.pMem[i] = pMem[i];
        }
    }
    else {
        for (size_t i = 0; i < MemLen; i++) {
            tmp.pMem[i] = pMem[i] | bf.pMem[i];
        }
        for (size_t i = MemLen; i < bf.MemLen; i++) {
            tmp.pMem[i] = bf.pMem[i];
        }
    }
    return tmp;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
    size_t MaxLen = max(BitLen, bf.BitLen);
    TBitField tmp(MaxLen);
    if (MemLen >= bf.MemLen) {
        for (size_t i = 0; i < bf.MemLen; i++) {
            tmp.pMem[i] = pMem[i] & bf.pMem[i];
        }
        for (size_t i = bf.MemLen; i < MemLen; i++) {
            tmp.pMem[i] = 0;
        }
    }
    else {
        for (size_t i = 0; i < MemLen; i++) {
            tmp.pMem[i] = pMem[i] & bf.pMem[i];
        }
        for (size_t i = MemLen; i < bf.MemLen; i++) {
            tmp.pMem[i] = 0;
        }
    }
    return tmp;
}

TBitField TBitField::operator~(void) // отрицание (Возможно перепишу)
{
    TBitField res(BitLen);
    for (size_t i = 0; i < MemLen; i++) {
        res.pMem[i] = ~pMem[i];
    }

    if (BitLen > 0) {
        size_t countAllBits = MemLen * (sizeof(TELEM) * 8);
        for (size_t i = BitLen; i < countAllBits; i++) {
            res.pMem[MemLen - 1] &= ~res.GetMemMask(i);
        }
    }

    return res;
}

// ввод/вывод

istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    char sumbol;
    int i = 0;
    while (i < bf.GetLength() && istr >> sumbol) {
        if (sumbol == '0')
            bf.ClrBit(i++);
        else if (sumbol == '1')
            bf.SetBit(i++);
        else {
            istr.putback(sumbol);
            break;
        }

    }
    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    for (int i = 0; i < bf.GetLength(); i++)
        ostr << bf.GetBit(i);
    return ostr;
}
