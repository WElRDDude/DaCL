from setuptools import setup, find_packages

setup(
    name="lab_car_video_system",
    version="0.1",
    packages=find_packages(),
    install_requires=[
        'opencv-python>=4.5.0',
        'numpy>=1.19.0',
        'python-can>=3.3.0',
        'RPi.GPIO>=0.7.0'
    ],
    entry_points={
        'console_scripts': [
            'lab_car_video=lab_car_video_system.main:main'
        ]
    },
    author="Your Name",
    author_email="your.email@example.com",
    description="A video recording system for lab cars with event triggering",
    license="MIT",
    keywords="video recording raspberry-pi can-bus",
    url="https://github.com/yourusername/lab_car_video_system",
)
