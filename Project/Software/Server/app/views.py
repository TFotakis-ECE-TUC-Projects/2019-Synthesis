from django.http import HttpResponse, JsonResponse
from django.shortcuts import render
from .detection import getData


# Create your views here.
def home(request):
	return render(request=request, template_name='Base/index.html')


def getTablesState(request):
	return JsonResponse(getData())
