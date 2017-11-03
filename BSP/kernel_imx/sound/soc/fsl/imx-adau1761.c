/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Copyright 2012 Linaro Ltd.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <sound/soc.h>

#include "../codecs/adau1761.h"
#include "../codecs/adau17x1.h"
#include "imx-audmux.h"

#define DAI_NAME_SIZE	32

//#define IMX_ADAU1761_DEBUG_DETAIL 1
#ifdef IMX_ADAU1761_DEBUG_DETAIL
#define DBG_DETAIL(format, arg...) \
	printk("*** %s *** " format "\n" , __func__, ## arg)
#else
#define DBG_DETAIL(format, arg...) do {} while (0)
#endif

struct imx_adau1761_data {
    struct snd_soc_dai_link dai[2];
    struct snd_soc_card card;
    char codec_dai_name[DAI_NAME_SIZE];
    char platform_name[DAI_NAME_SIZE];
    struct clk *codec_clk;
    unsigned int clk_frequency;
    bool is_codec_master;
    bool is_bt_master;
};

static int imx_adau1761_late_probe(struct snd_soc_card *card)
{
    struct snd_soc_dai *codec_dai = card->rtd[0].codec_dai;
    struct imx_adau1761_data *data = snd_soc_card_get_drvdata(card);
    int ret;

	DBG_DETAIL("freq=%d", data->clk_frequency);
    ret = snd_soc_dai_set_sysclk(codec_dai, ADAU17X1_CLK_SRC_MCLK,
            data->clk_frequency, SND_SOC_CLOCK_OUT);
	
    if (ret < 0)
        dev_err(NULL, "failed to set sysclk in %s\n", __func__);

    return ret;
}

static int imx_adau1x61_hw_params(struct snd_pcm_substream *substream,
    struct snd_pcm_hw_params *params)
{
    struct snd_soc_pcm_runtime *rtd = substream->private_data;
    struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_card *card = codec_dai->codec->card;
	struct imx_adau1761_data *data = snd_soc_card_get_drvdata(card);
    int pll_rate;
    int ret = 0;

	DBG_DETAIL();
    switch (params_rate(params)) {
    case 48000:
    case 8000:
    case 12000:
    case 16000:
    case 24000:
    case 32000:
    case 96000:
        pll_rate = 48000 * 1024;
        break;
    case 44100:
    case 7350:
    case 11025:
    case 14700:
    case 22050:
    case 29400:
    case 88200:
        pll_rate = 44100 * 1024;
        break;
    default:
        return -EINVAL;
    }

	if (data->is_codec_master) {
    	ret = snd_soc_dai_set_pll(codec_dai, ADAU17X1_PLL,
            ADAU17X1_PLL_SRC_MCLK, 12288000, pll_rate);
    	if (ret)
        	return ret;
	}

   	ret = snd_soc_dai_set_sysclk(codec_dai, ADAU17X1_CLK_SRC_MCLK, pll_rate,
           SND_SOC_CLOCK_OUT);
	
    return ret;
}

static struct snd_soc_ops imx_adau1x61_ops = {
    .hw_params = imx_adau1x61_hw_params,
};

static const struct snd_soc_dapm_widget imx_adau1761_dapm_widgets[] = {
    SND_SOC_DAPM_MIC("Mic Jack", NULL),
    SND_SOC_DAPM_LINE("Line In Jack", NULL),
    SND_SOC_DAPM_HP("Headphone Jack", NULL),
    SND_SOC_DAPM_SPK("Line Out Jack", NULL),
    SND_SOC_DAPM_SPK("Ext Spk", NULL),
};

static int imx_adau1761_audmux_config(struct platform_device *pdev,
                            struct imx_adau1761_data *data, int has_dev1)
{
    struct device_node *np = pdev->dev.of_node;
    int int_port, ext_port, int_port_bt, ext_port_bt;
    int ret;

    DBG_DETAIL("device1=%d", has_dev1);

    ret = of_property_read_u32(np, "mux-int-port-adau", &int_port);
    if (ret) {
        dev_err(&pdev->dev, "mux-int-port-adau missing or invalid\n");
        return ret;
    }
    ret = of_property_read_u32(np, "mux-ext-port-adau", &ext_port);
    if (ret) {
        dev_err(&pdev->dev, "mux-ext-port-adau missing or invalid\n");
        return ret;
    }

    if (has_dev1) {
        ret = of_property_read_u32(np, "mux-int-port-bt", &int_port_bt);
        if (ret) {
            dev_err(&pdev->dev, "mux-int-port-bt missing or invalid\n");
            return ret;
        }
        ret = of_property_read_u32(np, "mux-ext-port-bt", &ext_port_bt);
        if (ret) {
            dev_err(&pdev->dev, "mux-ext-port-bt missing or invalid\n");
            return ret;
        }

        int_port_bt--;
        ext_port_bt--;
    }

    /*
     * The port numbering in the hardware manual starts at 1, while
     * the audmux API expects it starts at 0.
     */
    int_port--;
    ext_port--;
    if (data->is_codec_master) {
        ret = imx_audmux_v2_configure_port(int_port,
            IMX_AUDMUX_V2_PTCR_SYN |
            IMX_AUDMUX_V2_PTCR_TFSEL(ext_port) |
            IMX_AUDMUX_V2_PTCR_TCSEL(ext_port) |
            IMX_AUDMUX_V2_PTCR_TFSDIR |
            IMX_AUDMUX_V2_PTCR_TCLKDIR,
            IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port));
    } else {
        ret = imx_audmux_v2_configure_port(int_port,
            IMX_AUDMUX_V2_PTCR_SYN,
            IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port));
    }
    if (ret) {
        dev_err(&pdev->dev, "audmux internal port setup failed\n");
        return ret;
    }

    if (data->is_codec_master) {
        ret = imx_audmux_v2_configure_port(ext_port,
            IMX_AUDMUX_V2_PTCR_SYN,
            IMX_AUDMUX_V2_PDCR_RXDSEL(int_port));
    } else {
        ret = imx_audmux_v2_configure_port(ext_port,
            IMX_AUDMUX_V2_PTCR_SYN |
            IMX_AUDMUX_V2_PTCR_TFSEL(int_port) |
            IMX_AUDMUX_V2_PTCR_TCSEL(int_port) |
            IMX_AUDMUX_V2_PTCR_TFSDIR |
            IMX_AUDMUX_V2_PTCR_TCLKDIR,
            IMX_AUDMUX_V2_PDCR_RXDSEL(int_port));
    }
    if (ret) {
        dev_err(&pdev->dev, "audmux external port setup failed\n");
        return ret;
    }

    if (has_dev1) {
        if (data->is_bt_master) {
            ret = imx_audmux_v2_configure_port(int_port_bt,
                IMX_AUDMUX_V2_PTCR_SYN |
                IMX_AUDMUX_V2_PTCR_TFSEL(ext_port_bt) |
                IMX_AUDMUX_V2_PTCR_TCSEL(ext_port_bt) |
                IMX_AUDMUX_V2_PTCR_TFSDIR |
                IMX_AUDMUX_V2_PTCR_TCLKDIR,
                IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port_bt));
        } else {
            ret = imx_audmux_v2_configure_port(int_port_bt,
                IMX_AUDMUX_V2_PTCR_SYN,
                IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port_bt));
        }
        if (ret) {
            dev_err(&pdev->dev, "audmux internal port for BT setup failed\n");
            return ret;
        }

        if (data->is_bt_master) {
            ret = imx_audmux_v2_configure_port(ext_port_bt,
                IMX_AUDMUX_V2_PTCR_SYN,
                IMX_AUDMUX_V2_PDCR_RXDSEL(int_port_bt));
        } else {
            ret = imx_audmux_v2_configure_port(ext_port_bt,
                IMX_AUDMUX_V2_PTCR_SYN |
                IMX_AUDMUX_V2_PTCR_TFSEL(int_port_bt) |
                IMX_AUDMUX_V2_PTCR_TCSEL(int_port_bt) |
                IMX_AUDMUX_V2_PTCR_TFSDIR |
                IMX_AUDMUX_V2_PTCR_TCLKDIR,
                IMX_AUDMUX_V2_PDCR_RXDSEL(int_port_bt));
        }
        if (ret) {
            dev_err(&pdev->dev, "audmux external port for BT setup failed\n");
            return ret;
        }
    }

    return 0;
}

static int imx_adau1761_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct device_node *codec_np, *cpu_np, *cpu_np1;
    struct platform_device *cpu_pdev, *cpu_pdev1;
    struct i2c_client *codec_dev;
    struct imx_adau1761_data *data = NULL;
    int ret;

    DBG_DETAIL();
    cpu_np = of_parse_phandle(np, "cpu-dai-adau", 0);
    if (!cpu_np) {
        dev_err(&pdev->dev, "phandle missing or invalid\n");
        ret = -EINVAL;
        goto fail;
    }

    cpu_np1 = of_parse_phandle(np, "cpu-dai-bt", 0);

    codec_np = of_parse_phandle(pdev->dev.of_node, "audio-codec", 0);
    if (!codec_np) {
        dev_err(&pdev->dev, "phandle missing or invalid\n");
        ret = -EINVAL;
        goto fail;
    }

    cpu_pdev = of_find_device_by_node(cpu_np);
    if (!cpu_pdev) {
        dev_err(&pdev->dev, "failed to find SSI platform device for device 0\n");
        ret = -EINVAL;
        goto fail;
    }

    if (cpu_np1) {
        cpu_pdev1 = of_find_device_by_node(cpu_np1);
        if (!cpu_pdev1) {
            dev_err(&pdev->dev, "failed to find SSI platform device for device 1\n");
            ret = -EINVAL;
            goto fail;
        }
    }

    codec_dev = of_find_i2c_device_by_node(codec_np);
    if (!codec_dev) {
        dev_err(&pdev->dev, "failed to find codec platform device\n");
        return -EPROBE_DEFER;
    }

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data) {
        printk(KERN_ALERT "imx-adau1761: kzalloc error\n");
        ret = -ENOMEM;
        goto fail;
    }

    if (of_property_read_bool(pdev->dev.of_node, "codec-master"))
        data->is_codec_master = true;
    if (of_property_read_bool(pdev->dev.of_node, "bt-master"))
        data->is_bt_master = true;

    if (strstr(cpu_np->name, "ssi")) {
        ret = imx_adau1761_audmux_config(pdev, data, cpu_np1);
        if (ret) {
            printk(KERN_ALERT "imx-adau1761: imx-audmux fail\n");
            goto fail;
        }	
    } else {
        printk(KERN_ALERT "imx-adau1761: not compatible name ssi2 <-> %s\n", cpu_np->name);
        goto fail;
    }

    data->codec_clk = devm_clk_get(&codec_dev->dev, "mclk");
    if (IS_ERR(data->codec_clk)) {
        dev_err(&codec_dev->dev, "could not get codec clk: %d\n", ret);
        ret = PTR_ERR(data->codec_clk);
        goto fail;
    }

    data->clk_frequency = clk_get_rate(data->codec_clk);
    clk_prepare_enable(data->codec_clk);

    data->dai[0].name = "adau1x61";
    data->dai[0].stream_name = "adau1x61";
    data->dai[0].codec_dai_name = "adau-hifi";
    data->dai[0].codec_of_node = codec_np;
    data->dai[0].cpu_dai_name = dev_name(&cpu_pdev->dev);
    data->dai[0].cpu_of_node = cpu_np;
    data->dai[0].platform_of_node = cpu_np;

//    data->dai.init = &imx_adau1761_dai_init;
    if (data->is_codec_master) {
        data->dai[0].dai_fmt = SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_NB_NF |
                SND_SOC_DAIFMT_CBM_CFM;
    } else {
        data->dai[0].dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
                SND_SOC_DAIFMT_CBS_CFS;
    }
    data->dai[0].ops = &imx_adau1x61_ops;
    data->card.num_links = 1;

    if (cpu_np1) {
        if (data->is_bt_master) {
            data->dai[1].dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
                SND_SOC_DAIFMT_CBM_CFM;
        } else {
            data->dai[1].dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
                SND_SOC_DAIFMT_CBS_CFS;
        }
        data->card.num_links = 2;
        data->dai[1].name = "BT";
        data->dai[1].stream_name = "I2S WL1835";
        data->dai[1].codec_name = "snd-soc-dummy";
        data->dai[1].codec_dai_name = "snd-soc-dummy-dai";
        data->dai[1].cpu_dai_name = dev_name(&cpu_pdev1->dev);
        data->dai[1].cpu_of_node = cpu_np1;
        data->dai[1].platform_of_node = cpu_np1;
    }

    data->card.dev = &pdev->dev;
    ret = snd_soc_of_parse_card_name(&data->card, "model");
    if (ret)
        goto fail;
//    ret = snd_soc_of_parse_audio_routing(&data->card, "audio-routing");
//    if (ret)
//        goto fail;
    data->card.owner = THIS_MODULE;
    data->card.dai_link = data->dai;
    data->card.dapm_widgets = imx_adau1761_dapm_widgets;
    data->card.num_dapm_widgets = ARRAY_SIZE(imx_adau1761_dapm_widgets);

    data->card.late_probe = imx_adau1761_late_probe;
    data->card.name = "musio-adau1761";

    platform_set_drvdata(pdev, &data->card);
    snd_soc_card_set_drvdata(&data->card, data);

    ret = devm_snd_soc_register_card(&pdev->dev, &data->card);
    if (ret) {
        dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n", ret);
        goto fail;
    }
    else{
        printk(KERN_ALERT "imx-adau1761: snd_soc_register_card ok\n");
    }

    of_node_put(cpu_np);
    of_node_put(codec_np);

    return 0;

fail:
    if (data && !IS_ERR(data->codec_clk))
        clk_put(data->codec_clk);
    of_node_put(cpu_np);
    of_node_put(codec_np);

    return ret;
}

static int imx_adau1761_remove(struct platform_device *pdev)
{
    struct snd_soc_card *card = platform_get_drvdata(pdev);
    struct imx_adau1761_data *data = snd_soc_card_get_drvdata(card);

	DBG_DETAIL();
    clk_put(data->codec_clk);

    return 0;
}

static const struct of_device_id imx_adau1761_dt_ids[] = {
    { .compatible = "fsl,imx-audio-adau1761", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_adau1761_dt_ids);

static struct platform_driver imx_adau1761_driver = {
    .driver = {
        .name = "imx-adau1761",
        .pm = &snd_soc_pm_ops,
        .of_match_table = imx_adau1761_dt_ids,
    },
    .probe = imx_adau1761_probe,
    .remove = imx_adau1761_remove,
};
module_platform_driver(imx_adau1761_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("Freescale i.MX ADAU1761 ASoC machine driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:imx-adau1761");
