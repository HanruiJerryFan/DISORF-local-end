# Code slightly adapted from Nerfstudio
# https://github.com/nerfstudio-project/nerfstudio/blob/df784e96e7979aaa4320284c087d7036dce67c28/nerfstudio/configs/method_configs.py

"""
Method configurations
"""

from __future__ import annotations

from typing import Dict

import tyro

from nerfstudio.cameras.camera_optimizers import CameraOptimizerConfig
from nerfstudio.configs.base_config import ViewerConfig
from nerfstudio.engine.optimizers import AdamOptimizerConfig
# from nerfstudio.models.nerfacto import NerfactoModelConfig
from nerfstudio.models.instant_ngp import InstantNGPModelConfig
from nerfstudio.pipelines.base_pipeline import VanillaPipelineConfig

from nsros.ros_datamanager import ROSDataManagerConfig
from nsros.ros_dataparser import ROSDataParserConfig
from nsros.ros_trainer import ROSTrainerConfig
from nsros.ros_pipeline import ROSPipelineConfig, ROSDynamicBatchPipelineConfig
from nsros.ros_nerfacto import NerfactoModelConfig

method_configs: Dict[str, ROSTrainerConfig] = {}
descriptions = {
    "ros_nerfacto": "Run the nerfstudio nerfacto method on data streamed from ROS.",
    "ros_instant_ngp": "Run the nerfstudio instant_ngp method on data streamed from ROS.",
}

method_configs["ros_nerfacto"] = ROSTrainerConfig(
    method_name="ros_nerfacto",
    steps_per_eval_batch=500000,
    steps_per_eval_image=500000,
    steps_per_save=8000,
    max_num_iterations=8000,
    mixed_precision=True,
    pipeline=ROSPipelineConfig(
        datamanager=ROSDataManagerConfig(
            dataparser=ROSDataParserConfig(
                aabb_scale=0.8, # pay attention [Ruofan]
            ),
            train_num_rays_per_batch=4096,
            eval_num_rays_per_batch=4096,
            camera_optimizer=CameraOptimizerConfig(
                mode="SO3xR3",
                optimizer=AdamOptimizerConfig(lr=6e-4, eps=1e-8, weight_decay=1e-2),
            ),
        ),
        model=NerfactoModelConfig(eval_num_rays_per_chunk=1 << 15),
    ),
    optimizers={
        "proposal_networks": {
            "optimizer": AdamOptimizerConfig(lr=1e-2, eps=1e-15),
            "scheduler": None,
        },
        "fields": {
            "optimizer": AdamOptimizerConfig(lr=1e-2, eps=1e-15),
            "scheduler": None,
        },
    },
    viewer=ViewerConfig(num_rays_per_chunk=20000),
    vis="viewer",
)

method_configs["ros_instant_ngp"] = ROSTrainerConfig(
    method_name="ros_instant_ngp",
    steps_per_eval_batch=10000,
    steps_per_eval_image=10000,
    steps_per_save=8000,
    max_num_iterations=8000,
    mixed_precision=True,
    pipeline=ROSDynamicBatchPipelineConfig(
        datamanager=ROSDataManagerConfig(
            dataparser=ROSDataParserConfig(),
            train_num_rays_per_batch=4096,
            eval_num_rays_per_batch=4096,
            camera_optimizer=CameraOptimizerConfig(
                mode="off",
                optimizer=AdamOptimizerConfig(lr=6e-4, eps=1e-8, weight_decay=1e-2),
            ),
        ),
        model=InstantNGPModelConfig(
            eval_num_rays_per_chunk=8192, 
            # disable_scene_contraction=True,
        )                           
    ),
    optimizers={
        "fields": {
            "optimizer": AdamOptimizerConfig(lr=1e-2, eps=1e-15),
            "scheduler": None,
        }
    },
    viewer=ViewerConfig(num_rays_per_chunk=1 << 12),
    vis="viewer",
)

AnnotatedBaseConfigUnion = (
    tyro.conf.SuppressFixed[  # Don't show unparseable (fixed) arguments in helptext.
        tyro.conf.FlagConversionOff[
            tyro.extras.subcommand_type_from_defaults(
                defaults=method_configs, descriptions=descriptions
            )
        ]
    ]
)
"""Union[] type over config types, annotated with default instances for use with
tyro.cli(). Allows the user to pick between one of several base configurations, and
then override values in it."""
