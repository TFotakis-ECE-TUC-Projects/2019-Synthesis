import random


def getData():
	return {
		'tables': [random.random() > 0.5 for _ in range(6)]
	}
