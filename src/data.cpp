#include "data.h"
#include "piece.h"
#include "square.h"

 const int    RedPawnInc[3 + 1]={ -16, -1, 1, 0 };
 const int    BlackPawnInc[3 + 1]={ 16, -1, 1, 0 };
 const int    KnightInc[8 + 1]={ -33, -31, -18, -14, +14, +18, +31, +33, 0 };
 const int    BishopInc[4 + 1]={ -34, -30, +30, +34, 0 };
 const int    RookInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    CannonInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    AdvisorInc[4 + 1]={ -17, -15, +15, +17, 0 };
 const int    KingInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    KnightLeg[8 + 1]={16, 16, 1, -1, 1, -1, -16, -16 , 0};

int          RankNonCapMax[9][512];
int          RankNonCapMin[9][512];
int          FileNonCapMax[10][1024];
int          FileNonCapMin[10][1024];

int          RookRankCapMax[9][512];
int          RookRankCapMin[9][512];
int          RookFileCapMax[10][1024];
int          RookFileCapMin[10][1024];

int          CannonRankCapMax[9][512];
int          CannonRankCapMin[9][512];
int          CannonFileCapMax[10][1024];
int          CannonFileCapMin[10][1024];

int          KingMoves[256][8];
int          AdvisorMoves[256][8];
int          BishopMoves[256][8];
int          ElephantEyes[256][4];
int          KnightMoves[256][12];
int          HorseLegs[256][8];
int          FileNonCapMoves[10][1024][12];
int          RankNonCapMoves[9][512][12];
int          RookFileCapMoves[10][1024][4];
int          RookRankCapMoves[9][512][4];
int          CannonFileCapMoves[10][1024][4];
int          CannonRankCapMoves[9][512][4];
int          PawnMoves[2][256][4];
int          PawnChecks[2][256][4];

int          NLegalDt[512];
int          KLegalDt[512];
int          ALegalDt[512];
int          BLegalDt[512];

int          PinNb[10][10][1024];
int          RankMoveNb[9][512];
int          FileMoveNb[10][1024];

/* */
static void init_move_nb();
static void init_pin();
static void init_dt();

/* */
void init_data()
{
	int  SrcSq, DstSq, Index, i, j, k;

	init_dt();
	init_pin();

	// ÿһ�У���һ��λ�飬��9�У���512�����
	for(i=0; i < 9; i++)
	{
		for(j=0; j < 512; j++)
		{
			Index=0;
			RankNonCapMax[i][j]=i + A0;//i��i�У�λ����j��������£��ǳ�������³�ʼλ��
			for(k=i + 1; k <= 8; k++)
			{
				if(j & (1 << k))//i������������k bit Ϊ1��ֹͣ
				{
					break;
				}
				//���λ����1,
				RankNonCapMoves[i][j][Index]=k + A0;//���ӷǳ��ӵ��ƶ�
				Index++;
				RankNonCapMax[i][j]=k + A0;//���·ǳ������λ�ã�
			}

			RankNonCapMin[i][j]=i + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					break;
				}

				RankNonCapMoves[i][j][Index]=k + A0;
				Index++;
				//���·ǳ�����Сλ��
				RankNonCapMin[i][j]=k + A0;
			}

			//��ӽ�����־0
			RankNonCapMoves[i][j][Index]=0;

			//���㳵��λ��
			Index=0;
			RookRankCapMax[i][j]=i + A0;
			for(k=i + 1; k <= 8; k++)
			{
				if(j & (1 << k))
				{
					RookRankCapMoves[i][j][Index]=k + A0;
					Index++;
					RookRankCapMax[i][j]=k + A0;
					break;
				}
			}

			RookRankCapMin[i][j]=i + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					RookRankCapMoves[i][j][Index]=k + A0;
					Index++;
					RookRankCapMin[i][j]=k + A0;
					break;
				}
			}

			RookRankCapMoves[i][j][Index]=0;

			//�����ڵ�λ��
			Index=0;
			CannonRankCapMax[i][j]=i + A0;
			for(k=i + 1; k <= 8; k++)
			{
				if(j & (1 << k))//�ұ���̨
				{
					k++;
					break;
				}
			}

			for(; k <= 8; k++)
			{
				if(j & (1 << k))//�ұ߳���
				{
					CannonRankCapMoves[i][j][Index]=k + A0;
					Index++;
					CannonRankCapMax[i][j]=k + A0;
					break;
				}
			}

			CannonRankCapMin[i][j]=i + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))//�����̨
				{
					k--;
					break;
				}
			}

			for(; k >= 0; k--)
			{
				if(j & (1 << k))//��߳���
				{
					CannonRankCapMoves[i][j][Index]=k + A0;
					Index++;
					CannonRankCapMin[i][j]=k + A0;
					break;
				}
			}

			CannonRankCapMoves[i][j][Index]=0;
		}
	}

	//���ÿÿ�У�������Ӧ��λ��
	for(i=0; i < 10; i++)
	{
		for(j=0; j < 1024; j++)
		{
			Index=0;
			FileNonCapMax[i][j]=(i << 4) + A0;
			for(k=i + 1; k <= 9; k++)
			{
				if(j & (1 << k))
				{
					break;
				}

				FileNonCapMoves[i][j][Index]=(k << 4) + A0;
				Index++;
				FileNonCapMax[i][j]=(k << 4) + A0;
			}

			FileNonCapMin[i][j]=(i << 4) + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					break;
				}

				FileNonCapMoves[i][j][Index]=(k << 4) + A0;
				Index++;
				FileNonCapMin[i][j]=(k << 4) + A0;
			}

			FileNonCapMoves[i][j][Index]=0;
			Index=0;
			RookFileCapMax[i][j]=(i << 4) + A0;
			for(k=i + 1; k <= 9; k++)
			{
				if(j & (1 << k))
				{
					RookFileCapMoves[i][j][Index]=(k << 4) + A0;
					Index++;
					RookFileCapMax[i][j]=(k << 4) + A0;
					break;
				}
			}

			RookFileCapMin[i][j]=(i << 4) + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					RookFileCapMoves[i][j][Index]=(k << 4) + A0;
					Index++;
					RookFileCapMin[i][j]=(k << 4) + A0;
					break;
				}
			}

			RookFileCapMoves[i][j][Index]=0;
			Index=0;
			CannonFileCapMax[i][j]=(i << 4) + A0;
			for(k=i + 1; k <= 9; k++)
			{
				if(j & (1 << k))
				{
					k++;
					break;
				}
			}

			for(; k <= 9; k++)
			{
				if(j & (1 << k))
				{
					CannonFileCapMoves[i][j][Index]=(k << 4) + A0;
					Index++;
					CannonFileCapMax[i][j]=(k << 4) + A0;
					break;
				}
			}

			CannonFileCapMin[i][j]=(i << 4) + A0;
			for(k=i - 1; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					k--;
					break;
				}
			}

			for(; k >= 0; k--)
			{
				if(j & (1 << k))
				{
					CannonFileCapMoves[i][j][Index]=(k << 4) + A0;
					Index++;
					CannonFileCapMin[i][j]=(k << 4) + A0;
					break;
				}
			}

			CannonFileCapMoves[i][j][Index]=0;
		}
	}

	//256�����ϣ�ÿ���ӵ�Ԥ���ɲ���
	for(SrcSq=0; SrcSq < 256; SrcSq++)
	{
		if(IN_CITY(SrcSq))
		{
			Index=0;
			for(i=0; i < 4; i++)
			{
				DstSq=SrcSq + KingInc[i];
				if(IN_CITY(DstSq))
				{
					KingMoves[SrcSq][Index]=DstSq;
					Index++;
				}
			}

			KingMoves[SrcSq][Index]=0;

			Index=0;
			for(i=0; i < 4; i++)
			{
				DstSq=SrcSq + AdvisorInc[i];
				if(IN_CITY(DstSq))
				{
					AdvisorMoves[SrcSq][Index]=DstSq;
					Index++;
				}
			}

			AdvisorMoves[SrcSq][Index]=0;
		}

		if(SquareTo90[SrcSq] != -1)
		{
			Index=0;
			for(i=0; i < 4; i++)
			{
				DstSq=SrcSq + BishopInc[i];
				if(SquareTo90[DstSq] != -1 && !((SrcSq ^ DstSq) & 0x80))
				{
					BishopMoves[SrcSq][Index]=DstSq;
					ElephantEyes[SrcSq][Index]=(SrcSq + DstSq) >> 1;
					Index++;
				}
			}

			BishopMoves[SrcSq][Index]=0;

			Index=0;
			for(i=0; i < 8; i++)
			{
				DstSq=SrcSq + KnightInc[i];
				if(SquareTo90[DstSq] != -1)
				{
					KnightMoves[SrcSq][Index]=DstSq;
					HorseLegs[SrcSq][Index]=SrcSq + NLegalDt[DstSq - SrcSq + 256];
					Index++;
				}
			}

			KnightMoves[SrcSq][Index]=0;

			for(i=0; i <= 1; i++)
			{
				Index=0;
				DstSq=i ? SrcSq + 16 : SrcSq - 16;
				if(SquareTo90[DstSq] != -1)
				{
					PawnMoves[i][SrcSq][Index]=DstSq;
					Index++;
				}

				if(i ? (SrcSq & 0x80) : !(SrcSq & 0x80))
				{
					for(j= -1; j <= 1; j+=2)
					{
						DstSq=SrcSq + j;
						if(SquareTo90[DstSq] != -1)
						{
							PawnMoves[i][SrcSq][Index]=DstSq;
							Index++;
						}
					}
				}

				PawnMoves[i][SrcSq][Index]=0;
			}

			for(i=0; i <= 1; i++)
			{
				Index=0;
				if(i == 0)
				{
					if(SrcSq < A6)
					{
						DstSq=SrcSq + 16;
						if(SquareTo90[DstSq] != -1)
						{
							PawnChecks[i][SrcSq][Index]=DstSq;
							Index++;
						}

						if(!(SrcSq & 0x80))
						{
							for(j= -1; j <= 1; j+=2)
							{
								DstSq=SrcSq + j;
								if(SquareTo90[DstSq] != -1)
								{
									PawnChecks[i][SrcSq][Index]=DstSq;
									Index++;
								}
							}
						}
					}

					PawnChecks[i][SrcSq][Index]=0;
				}
				else
				{
					if(SrcSq > I3)
					{
						DstSq=SrcSq - 16;
						if(SquareTo90[DstSq] != -1)
						{
							PawnChecks[i][SrcSq][Index]=DstSq;
							Index++;
						}

						if(SrcSq & 0x80)
						{
							for(j= -1; j <= 1; j+=2)
							{
								DstSq=SrcSq + j;
								if(SquareTo90[DstSq] != -1)
								{
									PawnChecks[i][SrcSq][Index]=DstSq;
									Index++;
								}
							}
						}
					}

					PawnChecks[i][SrcSq][Index]=0;
				}
			}
		}
	}

    init_move_nb();
}
//����λ��λ�У�����ĳ��λ�ã���ĳ��λ�л�λ������µ�����
static void init_move_nb()
{
	int i, j, ix;
	for(i=0; i < 9; ++i)
		for(j=0; j < 512; ++j)
		{
			ix=0;
			while(RankNonCapMoves[i][j][ix]) ix++;
			RankMoveNb[i][j]=ix;
		}

		for(i=0; i < 10; ++i)
			for(j=0; j < 1024; ++j)
			{
				ix=0;
				while(FileNonCapMoves[i][j][ix]) ix++;
				FileMoveNb[i][j]=ix;
			}
}

/* */
static void init_pin()
{
	int i, j, k, ix;
	int nb;
	for(i=0; i < 1024; ++i)
		for(j=0; j < 10; ++j)
			for(k=0; k < 10; ++k) PinNb[j][k][i]=0;
	for(i=0; i < 1024; ++i)
	{
		for(j=0; j < 10; ++j)
		{
			for(k=j + 2; k < 10; ++k)
			{
				nb=0;
				for(ix=j + 1; ix < k; ++ix)
				{
					if(i & (1 << ix)) nb++;//��λ�л�λ����i��������£���j��k֮�乲�����ӣ�
				}

				PinNb[j][k][i]=nb;
				PinNb[k][j][i]=nb;
			}
		}
	}
}

//���ݲ����ж�ĳ��λ���Ƿ����ܹ������Ƿ񱻽���������������Լӿ��ٶ�
static void init_dt()
{
	int i;
	for(i=0; i < 512; ++i)
	{
		KLegalDt[i]=0;
		ALegalDt[i]=0;
		BLegalDt[i]=0;
		NLegalDt[i]=0;
	}

	i=0;
	while(KingInc[i])
	{
		KLegalDt[KingInc[i] + 256]=KingInc[i];
		++i;
	}

	i=0;
	while(AdvisorInc[i])
	{
		ALegalDt[AdvisorInc[i] + 256]=AdvisorInc[i];
		++i;
	}

	i=0;
	while(BishopInc[i])
	{
		BLegalDt[BishopInc[i] + 256]=BishopInc[i];
		++i;
	}

	i=0;
	while(KnightInc[i])
	{
		NLegalDt[256 - KnightInc[i]]=KnightLeg[i];
		++i;
	}

}

