#include "adsimulate.h"
#include<google/protobuf/text_format.h>
#include<google/protobuf/io/zero_copy_stream_impl.h> 
#include <fcntl.h>
#include "vos_log.h"
#include "app_errorcode.h"


#ifdef COMPILER_IS_LINUX_GCC
CAdSimulate * g_pADSimulate = NULL;
#endif
CAdSimulate::CAdSimulate()
{

}

CAdSimulate::~CAdSimulate()
{

}

vos_u32 CAdSimulate::LoadCfg()
{
    int infile = open("adsimulate.cfg", O_RDONLY);

    if (infile < 0)
    {
        WRITE_ERR_LOG("open adsimulate.cfg failed.\n");
        return app_open_file_error;
    }

    google::protobuf::io::FileInputStream fileInput(infile);
    if (!google::protobuf::TextFormat::Parse(&fileInput, &m_stAdSimulate))
    {
        WRITE_ERR_LOG("Parse adsimulate.cfg failed.\n");
        close(infile);
        return app_open_file_error;
    }
    close(infile);
    return VOS_OK;
}

vos_u32 CAdSimulate::GetAdValue(vos_u32 taskId, vos_u32 channelIdx, int index)
{
	if (!m_stAdSimulate.data_size())
	{
		return 0;
	}

	int taskIdx = taskId%m_stAdSimulate.data_size();
    //取AD队列
    const ADValue& rAdValue = m_stAdSimulate.data(taskIdx);
    //主波
    if (0 == channelIdx % 2)
    {
        return index < rAdValue.uimainads_size() ? rAdValue.uimainads(index) : 0;
    }

    //副波
    return index < rAdValue.uisubads_size() ? rAdValue.uisubads(index) : 0;
}
