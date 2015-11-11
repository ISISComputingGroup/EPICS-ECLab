
BIOLOGIC_API(int) BL_GetLibVersionStub(char*  pVersion, unsigned int* psize);

//BIOLOGIC_API(int) BL_GetErrorMsgStub( int errorcode, char*  pmsg, unsigned int* psize ); 

BIOLOGIC_API(int) BL_ConnectStub(const char* address, uint8 timeout, int* pID, TDeviceInfos_t* pInfos); 

BIOLOGIC_API(int) BL_TestConnectionStub(int ID);

BIOLOGIC_API(int) BL_DisconnectStub(int ID);

BIOLOGIC_API(int) BL_LoadFirmwareStub( int ID, uint8* pChannels, int* pResults, uint8 Length, 
                             bool ShowGauge, bool ForceReload, const char* BinFile, const char* XlxFile);
							 
BIOLOGIC_API(bool) BL_IsChannelPluggedStub( int ID, uint8 ch ); 

BIOLOGIC_API(int) BL_GetChannelsPluggedStub ( int ID, uint8* pChPlugged, uint8 Size ); 

BIOLOGIC_API(int) BL_GetChannelInfosStub( int ID, uint8 ch, TChannelInfos_t* pInfos ); 

BIOLOGIC_API(int) BL_LoadTechniqueStub(int ID, uint8 channel, const char* pFName, TEccParams_t Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams);

BIOLOGIC_API(int) BL_UpdateParametersStub( int ID, uint8 channel, int TechIndx, TEccParams_t Params, const char* EccFileName ); 

BIOLOGIC_API(int) BL_StartChannelStub (int ID, uint8 channel);

BIOLOGIC_API(int) BL_StopChannelStub (int ID, uint8 channel);

BIOLOGIC_API(int) BL_GetCurrentValuesStub (int ID, uint8 channel, TCurrentValues_t *pValues); 

BIOLOGIC_API(int) BL_GetDataStub( int ID, uint8 channel, TDataBuffer_t* pBuf, TDataInfos_t* pInfos, TCurrentValues_t* pValues ); 

BIOLOGIC_API(int) BL_SetExperimentInfosStub( int ID, uint8 channel, TExperimentInfos_t TExpInfos );

BIOLOGIC_API(int) BL_GetExperimentInfosStub( int ID, uint8 channel, TExperimentInfos_t* TExpInfos );
