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
*
* ASoC machine driver imx6q-ssi <-> ak5386
* Based on imx-sgtl5000.c
*
*/

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <sound/soc.h>

#include "imx-audmux.h"

#define DAI_NAME_SIZE	32

struct imx_ak5386_data {
	struct snd_soc_dai_link dai;
	struct snd_soc_card card;
	char codec_dai_name[DAI_NAME_SIZE];
	char platform_name[DAI_NAME_SIZE];
};

static int imx_ak5386_dai_init(struct snd_soc_pcm_runtime *rtd) {
	return 0;
}

static int imx_ak5386_probe(struct platform_device *pdev) {
	struct device_node *np = pdev->dev.of_node;
	struct device_node *ssi_np, *codec_np;
	struct platform_device *ssi_pdev;
	struct imx_ak5386_data *data = NULL;
	int int_port, ext_port;
	int ret;

	ret = of_property_read_u32(np, "mux-int-port", &int_port);
	if (ret) {
		dev_err(&pdev->dev, "mux-int-port missing or invalid\n");
		return ret;
	}
	ret = of_property_read_u32(np, "mux-ext-port", &ext_port);
	if (ret) {
		dev_err(&pdev->dev, "mux-ext-port missing or invalid\n");
		return ret;
	}

	/*Inconsistency in datasheet and audmux API*/
	int_port--;
	ext_port--;
	ret = imx_audmux_v2_configure_port(int_port,
			IMX_AUDMUX_V2_PTCR_SYN |
			IMX_AUDMUX_V2_PTCR_TFSEL(ext_port) |
			IMX_AUDMUX_V2_PTCR_TCSEL(ext_port) |
			IMX_AUDMUX_V2_PTCR_TFSDIR |
			IMX_AUDMUX_V2_PTCR_TCLKDIR,
			IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port));
	if (ret) {
		dev_err(&pdev->dev, "audmux internal port setup failed\n");
		return ret;
	}
	ret = imx_audmux_v2_configure_port(ext_port,
			IMX_AUDMUX_V2_PTCR_SYN,
			IMX_AUDMUX_V2_PDCR_RXDSEL(int_port));
	if (ret) {
		dev_err(&pdev->dev, "audmux external port setup failed\n");
		return ret;
	}

	ssi_np = of_parse_phandle(pdev->dev.of_node, "ssi-controller", 0);
	codec_np = of_parse_phandle(pdev->dev.of_node, "audio-codec", 0);
	if (!ssi_np || !codec_np) {
		dev_err(&pdev->dev, "phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail;
	}

	ssi_pdev = of_find_device_by_node(ssi_np);
	if (!ssi_pdev) {
		dev_err(&pdev->dev, "failed to find SSI platform device\n");
		ret = -EPROBE_DEFER;
		goto fail;
	}

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto fail;
	}

	data->dai.name = "HiFi";
	data->dai.stream_name = "Capture";
	data->dai.codec_dai_name = "ak5386-hifi";
	data->dai.codec_of_node = codec_np;
	data->dai.cpu_of_node = ssi_np;
	data->dai.platform_of_node = ssi_np;
	data->dai.init = &imx_ak5386_dai_init;
	data->dai.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBM_CFM | 
			    SND_SOC_DAIFMT_NB_NF;

	data->card.dev = &pdev->dev;
	ret = snd_soc_of_parse_card_name(&data->card, "model");
	if (ret) {
		goto fail;
	}

	data->card.num_links = 1;
	data->card.owner = THIS_MODULE;
	data->card.dai_link = &data->dai;

	platform_set_drvdata(pdev, &data->card);
	snd_soc_card_set_drvdata(&data->card, data);

	ret = devm_snd_soc_register_card(&pdev->dev, &data->card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n", ret);
		goto fail;
	}

	of_node_put(ssi_np);
	of_node_put(codec_np);

	return 0;

fail:
	of_node_put(ssi_np);
	of_node_put(codec_np);

	return ret;

}

static int imx_ak5386_remove(struct platform_device *pdev) {

	return 0;
}

static const struct of_device_id imx_ak5386_dt_ids[] = {
	{ .compatible = "fsl,imx-audio-ak5386", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_ak5386_dt_ids);

static struct platform_driver imx_ak5386_driver = {
	.driver = {
		.name = "imx-ak5386",
		.pm = &snd_soc_pm_ops,
		.of_match_table = imx_ak5386_dt_ids,
	},
	.probe = imx_ak5386_probe,
	.remove = imx_ak5386_remove,
};
module_platform_driver(imx_ak5386_driver);

MODULE_AUTHOR("Nemanja Stefanovic");
MODULE_DESCRIPTION("IMX6 - AK5386 ASoC machine driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:imx-ak5386");
