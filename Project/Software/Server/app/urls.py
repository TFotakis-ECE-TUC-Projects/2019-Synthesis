from django.urls import path

from . import views

urlpatterns = [
	path('', views.home, name='home'),
	path('api/data/', views.getTablesState, name='getTablesState'),
]
